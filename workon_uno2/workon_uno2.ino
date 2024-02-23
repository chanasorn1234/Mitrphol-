#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include "DFRobot_MultiGasSensor.h"

#define I2C_COMMUNICATION
#define I2C_ADDRESS    0x74
DFRobot_GAS_I2C gas(&Wire ,I2C_ADDRESS);



#define ch4_pin A0 
#define arraySize 5
#define mq_135_pin A1
//float tx_msg[5];//idx 0:avg_co2 , 1:avg_voc , 2:pm2_5 , 3:pm10
bool receiving_data = false;
int sizeOfarray;
unsigned int pm2_5 = 0;
unsigned int pm10 = 0;

float latitude;
float lntitude;

String resultToMega[arraySize];
int countTofullArray = 0;

SoftwareSerial mega(2,3);
SoftwareSerial ss(4, 5);
TinyGPSPlus gps;
void setup() {
  pinMode(ch4_pin,INPUT);
  Serial.begin(115200);
  mega.begin(115200);
  ss.begin(115200);
 while(!gas.begin())
  {
    Serial.println("NO Deivces CO!");
    delay(1000);
  }
 
  gas.changeAcquireMode(gas.PASSIVITY);
  gas.setTempCompensation(gas.ON);
Serial.println("The device is connected successfully!");
}

void loop() {
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
 while (ss.available() > 0){
      gps.encode(ss.read());
      if (gps.location.isUpdated()){
      latitude = gps.location.lat();
      lntitude = gps.location.lng();
      Serial.print("Latitude= ");
      Serial.print(latitude, 6);
      Serial.print(" Longitude= ");
      Serial.println(lntitude, 6);
      }
  }

      latitude = 2;
      lntitude = 3;
      Serial.print("Latitude= ");
      Serial.print(latitude);
      Serial.print(" Longitude= ");
      Serial.println(lntitude);
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




















  
  ///add code comand to mega for sampling /// 
//  receiving_data = true;
//  
//  while(receiving_data == true){
//    while(mega.available()>0){
//      String receivedData = mega.readStringUntil('\n');
////      Serial.println(receivedData);
//      splitStringToArray(receivedData);
//
//    }
//  }
//  displayArray();
  
}


//void splitStringToArray(String input) {
//  int index = 0; // Index for the dataArray
//  int spaceIndex = 0; // Variable to store the position of the space character
////  Serial.println(input.substring(0, 6));
// 
//  while (spaceIndex != -1) {
//    spaceIndex = input.indexOf(","); // Find the position of the space character
//    
//    if(spaceIndex != -1){
//      rx_msg[index] = input.substring(0, spaceIndex).toFloat(); // Convert to float and add to array
//      input = input.substring(spaceIndex + 1); // Update input string to exclude the processed part
//      index++;
//    }
//  }
//  // Add the last element to the array
//  rx_msg[index] = input.toFloat();
//  receiving_data = false;
//  sizeOfarray = index+1;
//
//}
//
//void displayArray() {
//  Serial.println("Contents of the array:");
////  Serial.println(sizeof(rx_msg));
//  for (int i = 0; i < sizeOfarray; i++) {
//      Serial.println(rx_msg[i]);
//  }
//}
