// This file will be used for writing connection functions
// like WiFi, MQTT, etc.

#include "config.h"
#include <vector>
#include <ArduinoJson.h>

unsigned long lastAttemptTime = 0;

byte MCmacAdrr[6];
// --------------------------------------------------------
//                Parsing and storing tasks
// --------------------------------------------------------
String lastPayload = "";
std::vector<RawTask> rawTasks;
// Dummy JSON for testing parseJson and buildTree
// Example structure:
// - Two top-level tasks (parentId = "")
// - Each top-level task has two children (parentId = parent's task_id)

void parseJson(const String& payload) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (err) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(err.c_str());
        return;
    }
    JsonArray arr = doc.as<JsonArray>();

    rawTasks.clear();
    for (JsonObject o : arr) {
        RawTask r;
        r.list_id     = o["list_id"].as<String>();
        r.list_title  = o["list_title"].as<String>();
        r.task_id     = o["task_id"].as<String>();
        if (o["childOf"].isNull() || o["childOf"].as<String>() == "null") {
            r.parentId = "";
        } else {
            r.parentId = o["childOf"].as<String>();
        }
        r.notes       = o["notes"].as<String>();
        r.position    = o["position"].as<String>().toInt();
        r.title       = o["title"].as<String>();
        String statusStr = o["status"].as<String>();
        r.completed   = (statusStr == "completed"); // true if completed, false otherwise
        r.due         = o["due"].as<String>();
        rawTasks.push_back(r);
    }
    Serial.printf("Parsed %d tasks from JSON payload.\n", rawTasks.size());
}

std::vector<Task*> roots;
std::vector<Task*> allTasks;
void buildTree() {
    // 1) Create Task objects
    allTasks.clear();
    for (const auto& r : rawTasks) {
        Task* t = new Task{
        r.task_id,
        r.title,
        r.completed,
        r.due,
        r.notes,
        r.position,
        r.parentId,
        {}  // empty children
        };
        allTasks.push_back(t);
    }

    // 2) Attach children or collect roots
    roots.clear();
    for (auto* t : allTasks) {
        if (t->parentId.length() > 0) {
        // find parent by linear search
        for (auto* p : allTasks) {
            if (p->id == t->parentId) {
            p->children.push_back(t);
            break;
            }
        }
        } else {
        roots.push_back(t);
        }
    }

    // 3) Sort each node's children by position (simple bubble sort)
    auto sortLevel = [&](std::vector<Task*>& list) {
        int n = list.size();
        for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (list[j]->position > list[j+1]->position) {
            Task* tmp = list[j];
            list[j] = list[j+1];
            list[j+1] = tmp;
            }
        }
        }
    };

    // 4) Recursively sort
    std::function<void(std::vector<Task*>&)> recurseSort = [&](std::vector<Task*>& list) {
        sortLevel(list);
        for (auto* t : list) {
        if (t->children.size()) recurseSort(t->children);
        }
    };
    recurseSort(roots);
    Serial.printf("Built task tree with %d roots and %d total tasks.\n", roots.size(), allTasks.size());
}

// --------------------------------------------------------
//                        WI-Fi Setup
// --------------------------------------------------------
bool FirstWifiAttempt = true;
void setupWiFi(const char* ssid, const char* password) {
    WiFi.setHostname("Dzuu-ESP32");
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    unsigned long startAttemptTime = millis();
    const unsigned long wifiTimeout = 10000; // 10 seconds timeout

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout && !SPLASH_SCREEN) {
        if (FirstWifiAttempt) {
            Serial.print(".");
            delay(100);
        }
        // Add yield() to allow background tasks (non-blocking for ESP32)
        yield();
    }   

    if (WiFi.status() != WL_CONNECTED) {
        FirstWifiAttempt = false; // Set to false after first attempt
        Serial.println("\nWiFi connection failed!");
        return;
    }
    
    Serial.println("Successfully connected to WiFi!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC address: ");

    String macAdrr = WiFi.macAddress();

    // Place this array inside an extern variable to be used in other files
    // extern byte MCmacAdrr[6];
    sscanf(macAdrr.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
       &MCmacAdrr[0], &MCmacAdrr[1], &MCmacAdrr[2], 
       &MCmacAdrr[3], &MCmacAdrr[4], &MCmacAdrr[5]);

    Serial.println(macAdrr);
    Serial.println("\n");
    FirstWifiAttempt = true;
}


// --------------------------------------------------------
//                        MQTT Setup
// --------------------------------------------------------

const unsigned long retryInterval = 5000; // 5 seconds retry interval
void mqttConnect(const char* mqtt_server, const uint16_t mqtt_port,const char* topic) {
    mqtt.setBufferSize(8192); // Adjust size as needed
    mqtt.setServer(mqtt_server, mqtt_port);
    mqtt.setCallback(mqttCallback);
    // Non-blocking MQTT connect: try once, don't block if failed
    if (!mqtt.connected() && !SPLASH_SCREEN) {
        if (millis() - lastAttemptTime > retryInterval) {
            lastAttemptTime = millis(); // Update the last attempt time
            if (mqtt.connect("Dzuumirrah ESP32")) {
                Serial.println("Connecting to MQTT server...");
                Serial.printf("Connected to MQTT server at [%s:%d], subscribing to topic: [%s]\n", mqtt_server, mqtt_port, topic);
                if (!mqtt.subscribe(topic)) {
                    Serial.println("Failed to subscribe to topic.");
                }
                Serial.println("Connected to MQTT server!");
            } else {
                Serial.printf("Failed to connect to MQTT, rc=%d (will retry in %lu ms)\n", mqtt.state(), retryInterval);
            }
        }
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    // 1) Copy the payload into a String
    Serial.printf("MQTT message received on topic [%s]: ", topic);
    String incoming;
    for (unsigned int i = 0; i < length; i++) {
        incoming += char(payload[i]);
    }
    Serial.println(incoming);
    Serial.println("Parsing tasks . . .");
    // 2) Quick check: is it identical to last time?
    if (incoming == lastPayload) {
        // nothing changed → skip
        return;
    }

    // 3) It’s new or changed!
    lastPayload = incoming;         // remember it
    parseJson(incoming);            // fill rawTasks[]
    buildTree();                    // rebuild roots/allTasks]
    needDisplayUpdate = true; // Set flag to update display
}





