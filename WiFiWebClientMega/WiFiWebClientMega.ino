#include <SPI.h>
#include <WiFi.h>

#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#include <AFMotor.h>
#include <Servo.h>


#define LED_BLUE 13  
#define HALL_MARKER_SAMPLE_INTERVAL 10  //ms
#define MOTOR_MOVE_TIME HALL_MARKER_SAMPLE_INTERVAL*200 //ms - MUST BE AN MULTIPLE OF HALL_MARKER_SAMPLE_INTERVAL

// TODO: ******** NEED TO HAVE A LOWER THRESHOLD FOR OTHER POLARITY OF MAGNET!!!!!!!! (will be lower)  **********
#define HALL_DETECT_THRESHOLD 500 

#define CHASSIS4P0V1
//#define CHASSIS5P0V1

#ifdef CHASSIS4P0V1
//Configure drive motor for 4.0V1 chassis or 5.0V1 chassis
AF_DCMotor driveMotor(1, MOTOR12_64KHZ); //For chassis version 4.0V1
#endif

#ifdef CHASSIS5P0V1
AF_DCMotor driveMotor(3, MOTOR12_64KHZ);//For chassis version 5.0V1
#endif


//char ssid[] = "guest-SDUHSD"; //  your network SSID (name)
//char pass[] = "guest-SDUHSD";    // your network password (use for WPA, or use as key for WEP
char ssid[] = "Mauro Wi-Fi Network"; //  your network SSID (name)
char pass[] = "Maggie4848147";    // your network password (use for WPA, or use as key for WEP)

int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiClient client;


//Servo variables
Servo XYservo;
int XYservoPosition = 0;
Servo Zservo;
int ZservoPosition = 90;





int  hallSensorPin  =  A7;    // Hall effect analog input pin
int  hallSensorValue =  0;    // hall effect variable
///Setup bools so know what direction last traveled when detected Hall marker
//TODO:  make this persistent over power cycles
bool foundHallMarkerOnFwdMove = false;
bool foundHallMarkerOnRevMove = false;





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
Adafruit_MQTT_Subscribe motorForward = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/motorForward");
Adafruit_MQTT_Subscribe motorReverse = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/motorReverse");


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  pinMode(LED_BLUE, OUTPUT);  


//********************
/* This code is buggy -- destroys the wifi comm link for some reason....
  XYservo.attach(9);  //Corresponds to SERVO_2 on motor shield
  XYservo.write(XYservoPosition);
  Zservo.attach(10);  //Corresponds to SER1 on motor shield
  Zservo.write(ZservoPosition); */
//********************



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


  //Setup the MQTT subscriptions
  Serial.println("Setting up MQTT subscription");
  mqtt.subscribe(&testLED);
  mqtt.subscribe(&motorForward);
  mqtt.subscribe(&motorReverse);

}

void loop() {

   MQTT_connect();
   hallSensorValue =  analogRead(hallSensorPin);
   Serial.print("Hall value (loop): ");  Serial.println(hallSensorValue);


   //Rotate the servos for the 3-axis phone holder
   /* This code is buggy -- destroys the wifi comm link for some reason....

   Serial.println("Rotating servo postive");
   for (XYservoPosition = 0; XYservoPosition <=180; XYservoPosition++){
      XYservo.write(XYservoPosition);
      delay(10);
   }
   Serial.println("Rotating servo negative");
   for (XYservoPosition = 180; XYservoPosition >=0; XYservoPosition--){
      XYservo.write(XYservoPosition);
      delay(10);
   } */

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
    
    if (subscription == &motorForward) {
      //Stop motor if already running, move forward
      driveMotor.run(RELEASE);
      foundHallMarkerOnRevMove = false;  //Reset the reverse marker detect flag

      
      //First check if trying to move past a hall marker in the same direction 
      //as last detected the marker (can't do this!).
      if(!foundHallMarkerOnFwdMove){
        Serial.println("Motor forward");
        driveMotor.setSpeed(255);
        driveMotor.run(FORWARD);
        delay(400);  //move once in case positioned on top of the magnet

        for (int i = 0; i<MOTOR_MOVE_TIME; i+=HALL_MARKER_SAMPLE_INTERVAL) {
          hallSensorValue =  analogRead(hallSensorPin);
          Serial.print("Hall value: ");  Serial.println(hallSensorValue);
          if (hallSensorValue > HALL_DETECT_THRESHOLD){
            delay(HALL_MARKER_SAMPLE_INTERVAL);
            foundHallMarkerOnFwdMove = false;
          }
          else {
            foundHallMarkerOnFwdMove = true;
            driveMotor.run(RELEASE);
            Serial.println("Found a Hall marker!!!");
            delay(500);
            findTheHallMarker("forward"); //converge to the marker 
            break;
          }
        }        
      }
      driveMotor.run(RELEASE);
      Serial.println("Motor stopped");
    }
    
    if (subscription == &motorReverse) {
      //Stop motor if already running, move reverse
      driveMotor.run(RELEASE);
      foundHallMarkerOnFwdMove = false;  //Reset the forward marker detect flag

      //First check if trying to move past a hall marker in the same direction 
      //as last detected the marker (can't do this!).
      if(!foundHallMarkerOnRevMove){
        Serial.println("Motor reverse");
        driveMotor.setSpeed(255);
        driveMotor.run(BACKWARD);
        delay(400);  //move once in case positioned on top of the magnet
        for (int i = 0; i<MOTOR_MOVE_TIME; i+=HALL_MARKER_SAMPLE_INTERVAL) {
          hallSensorValue =  analogRead(hallSensorPin);
          Serial.print("Hall value: ");  Serial.println(hallSensorValue);
          if (hallSensorValue > HALL_DETECT_THRESHOLD){
            delay(HALL_MARKER_SAMPLE_INTERVAL);
            foundHallMarkerOnRevMove = false;
          }
          else {
            foundHallMarkerOnRevMove = true;
            driveMotor.run(RELEASE);
            Serial.println("Found a Hall marker!!!");
            delay(500);
            findTheHallMarker("reverse"); //connverge to the marker
            break;
          }
        }     
      }
      driveMotor.run(RELEASE);
      Serial.println("Motor stopped");
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

} //End Loop


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

  uint8_t retries = 10; 
  uint8_t savedRetries = retries;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.print("Error code: "); Serial.println(ret);
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         Serial.print("Tried to connect "); Serial.print (savedRetries); Serial.println(" times and Failed - Need to reset");
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

//Converge to the Hall marker (magnet)
void findTheHallMarker(String directionComingFrom){
  driveMotor.setSpeed(255);
  if(directionComingFrom.equals("forward")){
    driveMotor.run(BACKWARD);
    while(hallSensorValue =  analogRead(hallSensorPin) > HALL_DETECT_THRESHOLD)
      delay(10);  
              
    driveMotor.run(FORWARD);
    while(hallSensorValue =  analogRead(hallSensorPin) > HALL_DETECT_THRESHOLD)
      delay(10);    

    driveMotor.run(BACKWARD);
    while(hallSensorValue =  analogRead(hallSensorPin) > HALL_DETECT_THRESHOLD)
      delay(10);                          
  }
  else{
    driveMotor.run(FORWARD);
    while(hallSensorValue =  analogRead(hallSensorPin) > HALL_DETECT_THRESHOLD)
      delay(10);  
              
    driveMotor.run(BACKWARD);
    while(hallSensorValue =  analogRead(hallSensorPin) > HALL_DETECT_THRESHOLD)
      delay(10);    

    driveMotor.run(FORWARD);
    while(hallSensorValue =  analogRead(hallSensorPin) > HALL_DETECT_THRESHOLD)
      delay(10);                          
  }
  driveMotor.run(RELEASE);
  Serial.println("Converged to the Hall marker");
}


