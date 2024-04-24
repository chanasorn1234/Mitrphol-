#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include "DFRobot_MultiGasSensor.h"

#define I2C_COMMUNICATION
#define I2C_ADDRESS    0x74
DFRobot_GAS_I2C gas(&Wire ,I2C_ADDRESS);


///////////// pin ที่เอาไว้ต่อกับบอร์ด///////
#define ch4_pin A0 
#define mq_135_pin A1
#define sig_to_run 7
//////////////////////////////////////////

#define arraySize 5
//float tx_msg[5];//idx 0:avg_co2 , 1:avg_voc , 2:pm2_5 , 3:pm10
bool receiving_data = false;
bool system_run = false;

int sizeOfarray;
unsigned int pm2_5 = 0;
unsigned int pm10 = 0;

unsigned long startTime_gps = 0;
unsigned long timeoutDuration_gps = 1000; // 5000 milliseconds = 5 seconds

float latitude;
float lntitude;

String resultToMega[arraySize];
int countTofullArray = 0;

SoftwareSerial mega(2,3);
SoftwareSerial ss(4, 5);
TinyGPSPlus gps;
void setup() {
  pinMode(ch4_pin,INPUT);
  pinMode(sig_to_run,INPUT_PULLUP);
  pinMode(mq_135_pin,INPUT);
  Serial.begin(115200);
  mega.begin(115200);
  ss.begin(9600);
 while(!gas.begin())
  {
    Serial.println("NO Deivces CO!");
    delay(1000);
  }
 
  gas.changeAcquireMode(gas.PASSIVITY);
  gas.setTempCompensation(gas.ON);
Serial.println("The device is connected successfully!");
mega.flush();
}

void loop() {

  int input_from_D13 = digitalRead(sig_to_run);
//  Serial.println(input_from_D13);
  if(input_from_D13 == 0){
    system_run = true;
    Serial.println("System is running");
    }

  

  if(system_run == true){
      float avg_ch4 = 0;
      float avg_mq_135 = 0;
      countTofullArray = 0;
    
      //Methene
      for(int count_round=0;count_round<10;count_round++){ 
        float raw_data_ch4 = analogRead(ch4_pin);
        avg_ch4 += raw_data_ch4;
        delay(500);
        if(count_round == 9){
          avg_ch4 = avg_ch4/10;
          Serial.print("CH4: ");
          Serial.print(avg_ch4);
          Serial.println("PPM");
          resultToMega[countTofullArray]=avg_ch4;
          countTofullArray++;
          }
      }
    
      //Mq-135
      for(int count_round2=0;count_round2<10;count_round2++){ 
        float raw_data_mq_135 = analogRead(mq_135_pin);
        avg_mq_135 += raw_data_mq_135;
        delay(500);
        if(count_round2 == 9){
          avg_mq_135 = avg_mq_135/10;
          Serial.print("mq_135: ");
          Serial.print(avg_mq_135);
          Serial.println("PPM");
          resultToMega[countTofullArray]=avg_mq_135;
          countTofullArray++;
          }
      }
    
      //gps
      startTime_gps = millis();
     while(millis() - startTime_gps < timeoutDuration_gps){
//      Serial.println("wait for gps");
          if(ss.available()>0){
          gps.encode(ss.read());
          if (gps.location.isUpdated()){
            latitude = gps.location.lat();
            lntitude = gps.location.lng();
//            Serial.print("Latitude= ");
//            Serial.print(latitude, 6);
//            Serial.print(" Longitude= ");
//            Serial.println(lntitude, 6);
            break;
          }
      }
     }
//          latitude = 2;
//          lntitude = 3;
          Serial.print("Latitude= ");
          Serial.print(latitude,6);
          Serial.print(" Longitude= ");
          Serial.println(lntitude,6);
          resultToMega[countTofullArray]=latitude;
          countTofullArray++;
          resultToMega[countTofullArray]=lntitude;
          countTofullArray++;
    
      
      Serial.print("Ambient ");
      Serial.print(gas.queryGasType());
      Serial.print(" concentration is: ");
      Serial.print(gas.readGasConcentrationPPM());
      Serial.println(" PPM");
      resultToMega[countTofullArray]=gas.readGasConcentrationPPM();
      countTofullArray++;
    
//          mega.flush();
          Serial.println(countTofullArray);//set format to send data
          String combinedResult = "";
          if(countTofullArray == arraySize){
            for (int i = 0; i < arraySize; i++) {
              combinedResult += resultToMega[i];
              if (i < arraySize - 1) {
                combinedResult += ","; // Add a space between strings
              }
            }
          
            // Send the combined string through SoftwareSerial
            Serial.println(combinedResult);
            mega.print(combinedResult);
          }
    
    system_run = false;
    Serial.println("System is done");
    delay(10000);
  }
}
