/*
  DigitalReadSerial
 Reads a digital input on pin 2, prints the result to the serial monitor

 This example code is in the public domain.
 */

// digital pin 2 has a pushbutton attached to it. Give it a name:
int pushButton = 5;
int array[] = {0};
int count = 0;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(2000000);
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT);
}
bool pushed = false;
void loop() {
  int buttonState = digitalRead(pushButton);
  if (buttonState == 0){ 
   pushed=true;}

  if (pushed==true){
      if(count<20000){
        array[count] = int(digitalRead(pushButton));
        count++;}
      else{

        for(int i = 3250; i < 4090; i++)
          {
           Serial.print(String(array[i]));

          }
                     Serial.println("");
          pushed=false;
          count=0;
          delay(100);
        }
  }
}



