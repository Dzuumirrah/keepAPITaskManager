// This file will be used for writing connection functions
// like WiFi, MQTT, etc.

#include "config.h"
#include <vector>
#include <ArduinoJson.h>

byte MCmacAdrr[6];
// --------------------------------------------------------
//                Parsing and storing tasks
// --------------------------------------------------------
String lastPayload = "";
std::vector<RawTask> rawTasks;
void parseJson(const String& payload) {
    JsonDocument doc;
    deserializeJson(doc, payload);
    JsonArray arr = doc.as<JsonArray>();

    rawTasks.clear();
    for (JsonObject o : arr) {
        RawTask r;
        r.list_id     = o["list_id"].as<String>();
        r.list_title  = o["list_title"].as<String>();
        r.task_id     = o["task_id"].as<String>();
        r.parentId    = o["childOf"].as<String>();
        r.notes       = o["notes"].as<String>();
        r.position    = o["position"].as<int>();
        r.title       = o["title"].as<String>();
        r.completed   = o["status"].as<bool>();
        r.due         = o["due"].as<String>();
        rawTasks.push_back(r);
    }
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
}

// --------------------------------------------------------
//                        WI-Fi Setup
// --------------------------------------------------------

void setupWiFi(const char* ssid, const char* password) {
    WiFi.setHostname("Dzuu-ESP32");
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        
        Serial.print(".");
        delay(500);
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
    Serial.println("\n\n\n\n");
}


// --------------------------------------------------------
//                        MQTT Setup
// --------------------------------------------------------

void mqttConnect(const char* mqtt_server, const uint16_t mqtt_port,const char* topic) {
    mqtt.setBufferSize(512); // Adjust size as needed
    mqtt.setServer(mqtt_server, mqtt_port);
    mqtt.setCallback(mqttCallback);
    Serial.println("Connecting to MQTT server...");
    while (!mqtt.connected()) {
        if (mqtt.connect("Dzuumirrah ESP32")) {
            Serial.println(".");
            mqtt.subscribe(topic); // Subscribe to a topic
        } else {
            Serial.print("Failed, rc=");
            Serial.print(mqtt.state());
            Serial.println(" try again in 2 seconds");
            delay(2000);
        }
    }
    Serial.println("Connected to MQTT server!");
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    // 1) Copy the payload into a String
    String incoming;
    for (unsigned int i = 0; i < length; i++) {
        incoming += char(payload[i]);
    }

    // 2) Quick check: is it identical to last time?
    if (incoming == lastPayload) {
        // nothing changed → skip
        return;
    }

    // 3) It’s new or changed!
    lastPayload = incoming;         // remember it
    parseJson(incoming);            // fill rawTasks[]
    buildTree();                    // rebuild roots/allTasks
}





