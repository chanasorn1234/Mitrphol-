#include <SoftwareSerial.h>
unsigned int pm1 = 0;
unsigned int pm2_5 = 0;
unsigned int pm10 = 0;
//SoftwareSerial ss(15,14);
void setup() {
Serial.begin(9600);
Serial2.begin(9600);//pm sensor rx,tx on pin 17,16
//ss.begin(9600);
}
 
void loop() {
int index = 0;
char value;
char previousValue;
 
while (Serial2.available()) {
  value = Serial2.read();
  if ((index == 0 && value != 0x42) || (index == 1 && value != 0x4d)){
  Serial.println("Cannot find the data header.");
  break;
  }
   
  if (index == 4 || index == 6 || index == 8 || index == 10 || index == 12 || index == 14) {
  previousValue = value;
  }
  else if (index == 5) {
    pm1 = 256 * previousValue + value;
    Serial.print("{ ");
    Serial.print("\"pm1\": ");
    Serial.print(pm1);
    Serial.print(" ug/m3");
    Serial.print(", ");
  }
  else if (index == 7) {
    pm2_5 = 256 * previousValue + value;
    Serial.print("\"pm2_5\": ");
    Serial.print(pm2_5);
    Serial.print(" ug/m3");
    Serial.print(", ");
  }
  else if (index == 9) {
    pm10 = 256 * previousValue + value;
    Serial.print("\"pm10\": ");
    Serial.print(pm10);
    Serial.print(" ug/m3");
  } 
  else if (index > 15) {
  break;
  }
  index++;
}
while(Serial2.available()) Serial2.read();
Serial.println(" }");
delay(1000);
}
