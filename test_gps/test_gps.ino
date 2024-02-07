#include <TinyGPS++.h>
#include <SoftwareSerial.h>

SoftwareSerial ss(3, 4);
TinyGPSPlus gps;
void setup() {
  Serial.begin(9600);
  ss.begin(9600);

}

void loop() {
  while (ss.available() > 0){
    gps.encode(ss.read());
    if (gps.location.isUpdated()){
    // Latitude in degrees (double)
    Serial.print("Latitude= ");
    Serial.print(gps.location.lat(), 6);
    // Longitude in degrees (double)
    Serial.print(" Longitude= ");
    Serial.println(gps.location.lng(), 6);
    Serial.println(gps.date.value());
//    Serial.println(gps.time.value());
Serial.print("Hour = ");
Serial.println(gps.time.hour()+7);
// Minute (0-59) (u8)
Serial.print("Minute = ");
Serial.println(gps.time.minute());
// Second (0-59) (u8)
Serial.print("Second = ");
Serial.println(gps.time.second());
        }

}
}
