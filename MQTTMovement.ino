#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include <AFMotor.h>

#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#define LED_BLUE 13  

char ssid[] = "guest-SDUHSD"; //  your network SSID (name)
char pass[] = "guest-SDUHSD";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

char mqtt_server = "m10.cloudmqtt.com:15345";

char server[] = "www.google.com";

int status = WL_IDLE_STATUS;
long lastMsg = 0;
char msg[50];
int value = 0;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.

WiFiClient wifiClient;
PubSubClient client(wifiClient);

AF_DCMotor motor(1, MOTOR12_64KHZ);

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  if(topic == "Commands"){
    if(payload == "left"){
      motor.setSpeed(50);
      motor.run(FORWARD);
    }else if(payload == "right"){
      motor.setSpeed(50);
      motor.run(BACKWARD);
    }else{
      motor.run(RELEASE):
    }
  }
  
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "Arduino-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect("Arduino","Arduino","arduino")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("Output", "hello Arduino");
      // ... and resubscribe
      client.subscribe("Commands");
      Serial.println("Subscribed to commands");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(500);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer("m10.cloudmqtt.com", 15345);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

//  long now = millis();
//  if (now - lastMsg > 2000) {
//    lastMsg = now;
//    ++value;
//    snprintf (msg, 75, "hello world", value);
//    Serial.print("Publish message: ");
//    Serial.println(msg);
//    client.publish("Output", msg);
//  }
}  
