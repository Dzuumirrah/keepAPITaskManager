import paho.mqtt.client as mqtt
import os

mqtt_broker             = os.getenv("MQTT_BROKER")
mqtt_port               = int(os.getenv("MQTT_PORT"))
mqtt_topic              = os.getenv("MQTT_TOPIC_T")

mqttClient = mqtt.Client(client_id="Subscriber")

@mqttClient.connect_callback()
def on_connect(client, userdata, connect_flags, reason_code, properties):
    print("[MQTT] Connected with result code [", str(reason_code), "]")
    print(f"[MQTT] Subscribing to topic: [{mqtt_topic}]")
    print("[MQTT] Waiting for messages...\n")
    mqttClient.subscribe(mqtt_topic)

@mqttClient.message_callback()
def on_message(client, userdata, msg):
    print(f"[MQTT] Received message on topic [{msg.topic}] {msg.payload.decode()} \n\n")
    # payload = {   "list_id": "d01JRDdLdnVhMzg0eHl3Qw",                    -> identifier of the Task List name
    #               "list_title": "Tugas Kuliah",                           -> title of the Task List name
    #               "task_id": "ZEVwMEVwVXNydWN3QmlRLQ",                    -> identifier of the Task
    #               "childOf": "ZjNaRWJMTEU4Z2hJLTQtdQ",                    -> identifier of child from which parent Task. If null, it is the parent
    #               "position": "00000000000000000001",                     -> position of the Task in the list based on the order of the Task manually
    #               "title": "[Laprak] Laporan Praktikum LS 3 + simulasi",  -> title of the Task
    #               "status": "needsAction",                                -> status of the Task
    #               "created": null,                                        -> creation date of the Task
    #               "updated": "2025-05-22T10:40:46.962Z",                  -> last updated date of the Task as a RFC 3339 timestamp
    #               "due": null}                                            -> due date of the Task as a RFC 3339 timestamp
@mqttClient.disconnect_callback()
def disconnect_callback(client, userdata, disconnect_flags, reason_code, properties):
    print("[MQTT] Disconnected with result code [", str(reason_code), "]")

mqttClient.connect(mqtt_broker, int(mqtt_port), 60)
mqttClient.loop_forever()
