
#include <SPI.h>
#include <WiFi.h>

#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#define LED_BLUE 13  

char ssid[] = "guest-SDUHSD"; //  your network SSID (name)
char pass[] = "guest-SDUHSD";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiClient client;


/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "amauro"
#define AIO_KEY         "09608c4cd13645118217c89e278a855a"


// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Subscribe testLED = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/testLED");


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  pinMode(LED_BLUE, OUTPUT);  

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();


  //Setup the MQTT subscription
  Serial.println("Setting up MQTT subscription");
  mqtt.subscribe(&testLED);
}

void loop() {

   MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // readSubscription (int16_t timeout)
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {

    // Check if it's the testLED feed )
    if (subscription == &testLED) {
      Serial.print(F("TestLED: "));
      Serial.println((char *)testLED.lastread);
      
      if (strcmp((char *)testLED.lastread, "ON") == 0) {
        digitalWrite(LED_BLUE, HIGH); 
      }
      if (strcmp((char *)testLED.lastread, "OFF") == 0) {
        digitalWrite(LED_BLUE, LOW); 
      }
    }
  }
  
//    Serial.println("Pinging the server");
//    pingTheServer();
    
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

  }

  printWifiStatus();

}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  // mqtt connect api's:  https://pubsubclient.knolleary.net/api.html#connected
  bool connected = mqtt.connected();
  if(connected) {
    delay(3000);
    Serial.println("MQTT is connected");
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.print("Error code: "); Serial.println(ret);
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         Serial.println("Tried to connect 3 times and Failed - Need to reset");
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

void pingTheServer() {
  unsigned long StartTime = millis();
  if (!mqtt.ping()) {
    Serial.println("PING FAILURE");
    mqtt.disconnect();
  }else{
    unsigned long elapsedTime= millis() - StartTime;
    Serial.print("ping() took: ");
    Serial.print(elapsedTime);
    Serial.println(" ms");
  }
  
}





