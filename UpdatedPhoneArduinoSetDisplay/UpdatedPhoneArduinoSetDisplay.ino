#include <SPI.h>
#include <Wire.h>
#include <AndroidAccessory.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

AndroidAccessory acc("Arduino", "Mega_ADK", "X", "1.0", "X", "X");

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16
#define  LED_PIN  13 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

void setup()   {                
  Serial.begin(115200);
  acc.powerOn();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  pinMode(LED_PIN, OUTPUT);
  // Shows the adafruit logo at start
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
  delay(2000);
}


void loop() {
  delay(1000);
  byte msg[4];
  if (acc.isConnected()) {
    display.display();
    int len = acc.read(msg, sizeof(msg), 567);
    Serial.print("Bytes received: "); 
    Serial.print(msg[0]); Serial.print(", ");
    Serial.print(msg[1]); Serial.print(", ");
    Serial.print(msg[2]); Serial.print(", ");
    Serial.println(msg[3]);
    if(len >0){
      if(msg[0] == 1 && msg[1] == 0){
      for (int16_t i=0; i<display.height(); i+=2) {
        display.drawCircle(display.width()/2, display.height()/2, i, WHITE);
        display.display();
        delay(1);
      }
      }else if(msg[0] == 1 && msg[1] == 1){
        display.clearDisplay();
      }else if(msg[0] == 0 && msg[1] == 1){
//        display.setCursor(0,0);
//        display.print("This is a test");
// This test did not end up working
      }else if(msg[0] == 15){
        byte mesToSend[4] = {(byte)5,(byte)4,(byte)3,(byte)2};
        int bytesSent = acc.write(mesToSend,4);
        Serial.print("Bytes sent: "); Serial.println(bytesSent);
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(15,0);
        display.println("Command #15");
        display.display();
//        for (int16_t i=0; i<display.height(); i+=2) {
//           display.drawCircle(display.width()/2, display.height()/2, i, WHITE);
//           display.display();
//           delay(1); }

        
      }else if(msg[0] == 1) {
          byte mesToSend[6] = {(byte)0,(byte)2,(byte)4,(byte)6,(byte)8,(byte)10};
          int bytesSent = acc.write(mesToSend,6);
          display.clearDisplay();
          display.setCursor(0,0);
          display.print("Command #1");
      }else {
          byte mesToSend[6] = {(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0};
          int bytesSent = acc.write(mesToSend,6);
          display.clearDisplay();
          display.setCursor(0,0);
          display.print("Unknown Command");
      }
      
    }
  }
}

