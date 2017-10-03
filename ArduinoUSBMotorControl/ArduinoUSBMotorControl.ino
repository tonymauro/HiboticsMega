#include <AndroidAccessory.h>
#include <AFMotor.h>

char accessoryName[] = "Mega_ADK";
char companyName[] = "Arduino";

AndroidAccessory usb(companyName, accessoryName);
AF_DCMotor motor(1, MOTOR12_64KHZ);

int time = 0;

boolean flag = false;
void setup() {
  
  Serial.begin(9600);
  usb.begin();
  Serial.setTimeout(50);
  // put your setup code here, to run once:

}

void loop() {
  int msg[2];
  time++;
  if(time > 5000){
    time = 0;
  }
  // put your main code here, to run repeatedly:
  if(flag){
    msg[0] = usb.read();
    msg[1] = usb.read();  
    byte mesToSend[4] = {(byte)5,(byte)4,(byte)3,(byte)2};
 //     int bytesSent = usb.write(mesToSend,4);
//      Serial.println(msg[0]);
//      Serial.println(msg[1]);
//      Serial.println(msg[2]);
//      Serial.println(msg[3]);

    Serial.println(time);
    
    if(msg[0] == 164){
      if(msg[1] == 5){
          Serial.println("Before forward");
          motor.setSpeed(200);
          motor.run(FORWARD);
          Serial.println("Sending forward message");
      }else if(msg[1] == 7){
        Serial.println("Before Stop");
        motor.setSpeed(0);
        motor.run(RELEASE);
      }else if(msg[1] == 6){
        Serial.println("Before backward");
        motor.setSpeed(200);
        motor.run(BACKWARD); 
        Serial.println("Sending backward message");
      }
    }

  }else{
    Serial.println("Not connected");
    if(usb.isConnected()) flag = true;
  }
//    msg[0] = usb.read();
//    msg[1] = usb.read();  
//    byte mesToSend[4] = {(byte)5,(byte)4,(byte)3,(byte)2};
//      int bytesSent = usb.write(mesToSend,4);
//      Serial.println(msg[0]);
//      Serial.println(msg[1]);
//      Serial.println(msg[2]);
//      Serial.println(msg[3]);
//
//    Serial.println(time);
//    
//    if(msg[0] == 164){
//      if(msg[1] == 5){
//        if(flag){
//          Serial.println("Before forward");
//          motor.run(FORWARD);
//          Serial.println("Sending forward message");
//        }else{
//          
//        }
//      }else if(msg[1] == 7){
//        Serial.println("Before Stop");
//        motor.run(RELEASE);
//        
//      }
//    }
//    
//    Serial.println("we are connected");
//    
//  }else{
//    Serial.println("we are not connected");
//  }
}
