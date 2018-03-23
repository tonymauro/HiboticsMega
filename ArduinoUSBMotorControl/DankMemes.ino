#include <MegaIOT.h>
#include <SPI.h>

  ProgQueue tester;
int next(int n){
  Serial.print(n);
  return (n+1);
}

int merp(uint16_t inputs){
  return 1;
}
int wee(uint16_t inputs){
  Serial.print("bai bai");
  return 0;
}
/*int while(int n){
  if(lcond){
    return n+1;
  }

}*/
int done(int n) {
  Serial.print("merp, ");
  
  return n;
}

void setup() {
  Serial.begin(57600);
  tester.setCMDef(100, merp, next);
  tester.setCMDef(23, wee, done);
  uint16_t merp[] = {100,2,1};
  uint16_t wee[] = {23,1,1};
  tester.setCMD(0, merp);
  tester.setCMD(1, wee);

  
}

void loop() {
  tester.loop();
  delay(600);

}

