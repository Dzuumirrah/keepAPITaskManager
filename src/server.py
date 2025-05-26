# To subscribe to the MQTT topic, use the following command:
# mosquitto_sub.exe -h localhost -p 1883 -t test/topic
# or execute "subscriber.py" in another terminal


import paho.mqtt.client as mqtt
import os
from dotenv import load_dotenv
import json
import time


from google_auth_oauthlib.flow import InstalledAppFlow
from google.oauth2.credentials import Credentials
from googleapiclient.discovery import build
from google.auth.transport.requests import Request

# -------------------------------------------------------------------
# CONFIGURATION
# -------------------------------------------------------------------
load_dotenv()
SCOPES = ['https://www.googleapis.com/auth/tasks']
CREDENTIALS_FILE = 'GoogleCredential\\credentials.json'  # downloaded from Google Console
TOKEN_FILE       = 'GoogleCredential\\token.json'
# Load environment variables from .env file
# MQTT topics
mqtt_broker             = os.getenv("MQTT_BROKER")
mqtt_port               = int(os.getenv("MQTT_PORT"))
mqtt_topic              = os.getenv("MQTT_TOPIC_T")

# -------------------------------------------------------------------
# Google Keep and MQTT Initialization
# -------------------------------------------------------------------
def get_credentials():
    creds = None
    # Load existing token if present
    if os.path.exists(TOKEN_FILE):
        creds = Credentials.from_authorized_user_file(TOKEN_FILE, SCOPES)
    # If no valid credentials, run through OAuth flow
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file(
                CREDENTIALS_FILE, SCOPES)
            creds = flow.run_local_server(port=0)
        # Save the credentials for next run
        with open(TOKEN_FILE, 'w') as token:
            token.write(creds.to_json())
    return creds

# initialize MQTT client
mqttClient = mqtt.Client(client_id="MQTTGoogleKeepESP32")

# -------------------------------------------------------------------
# MQTT Callbacks
# -------------------------------------------------------------------
@mqttClient.connect_callback()
def on_connect(client, userdata, connect_flags, reason_code, properties):
    print("! [MQTT] Connected with result code ", str(reason_code), '\n\n')
    client.subscribe(mqtt_topic)

# -------------------------------------------------------------------
# MQTT Connection
# -------------------------------------------------------------------
mqttClient.connect(mqtt_broker, int(mqtt_port), 60)

# -------------------------------------------------------------------
# Fetch all tasks from Google Tasks
# -------------------------------------------------------------------
def fetch_all_tasks(creds):
    service = build('tasks', 'v1', credentials=creds)
    tasks_data = []
    # first get all task-lists
    tasklists = service.tasklists().list().execute().get('items', [])
    for tl in tasklists:
        tl_id = tl['id']
        tl_title = tl.get('title', 'Unnamed List')
        # now fetch tasks for this list
        tasks = service.tasks().list(tasklist=tl_id).execute().get('items', [])
        for t in tasks:
            tasks_data.append({
                'list_id':      tl_id,
                'list_title':   tl_title,
                'task_id':      t.get('id'),
                'childOf':      t.get('parent'),
                'position':     t.get('position'),
                'notes':        t.get('notes'),
                'title':        t.get('title'),
                'status':       t.get('status'),
                'created':      t.get('created'),
                'updated':      t.get('updated'),
                'due':          t.get('due'),
            })
    return tasks_data

# -------------------------------------------------------------------
# Publish tasks over MQTT
# -------------------------------------------------------------------
def publish_tasks(tasks):
    # Publish all tasks as a single JSON message
    payload = json.dumps(tasks, sort_keys=True)
    # Publish the tasks to the MQTT topic
    mqttClient.publish(mqtt_topic, payload, retain=True)
    print(f"* [MQTT] Published all tasks ({len(tasks)}) to MQTT topic '{mqtt_topic}'")

def PrintTasks(tasks):
    print(f"* [SERVER] Fetched {len(tasks)} tasks:")
    for t in tasks:
        if t['childOf'] is None:
            # Only show top-level tasks
            print(f"{tasks.index(t)+1}. {t['title']}")
            for subtask in tasks:
                if subtask['childOf'] == t['task_id']:
                    print(f"  • {subtask['title']}")
            

# -------------------------------------------------------------------
# MAIN ENTRYPOINT
# -------------------------------------------------------------------
def main():
    last_published_JSON = "" # store last published JSON to avoid duplicates
    mqttClient.loop_start()

    while True:
        try:
            # Authenticate
            creds = get_credentials()
            # Fetch tasks into json format
            tasks = fetch_all_tasks(creds)

            current_JSON = json.dumps(tasks, sort_keys=True)

            if current_JSON == last_published_JSON:
                print("* [MQTT] NO CHANGE DETECTED.")
            else:
                print("! [MQTT] CHANGE DETECTED, publishing tasks...")
                # Print tasks in a readable format
                PrintTasks(tasks)
                # Publish via MQTT
                publish_tasks(tasks)
                last_published_JSON = current_JSON

            time.sleep(10)  # Wait for 10 seconds before fetching tasks again
        except KeyboardInterrupt:
            print("! [MQTT] Stopping the MQTT client...")
            mqttClient.loop_stop()
            break

if __name__ == '__main__':
    main()
