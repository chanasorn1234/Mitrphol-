
////////////////Part of SEN55-SDN-T module////////////////
#include <Arduino.h>
#include <SensirionI2CSen5x.h>
#include <Wire.h>

// The used commands use up to 48 bytes. On some Arduino's the default buffer
// space is not large enough
#define MAXBUF_REQUIREMENT 48

#if (defined(I2C_BUFFER_LENGTH) &&                 \
     (I2C_BUFFER_LENGTH >= MAXBUF_REQUIREMENT)) || \
    (defined(BUFFER_LENGTH) && BUFFER_LENGTH >= MAXBUF_REQUIREMENT)
#define USE_PRODUCT_INFO
#endif

SensirionI2CSen5x sen5x;

////////////////End Part of SEN55-SDN-T module////////////////


///////////// pin ที่เอาไว้ต่อกับบอร์ด///////
#define carbon_pin  A1
#define voc_pin  A2
#define sig_to_runUno 7
#define led_status1 2
#define led_status2 3
//////////////////////////////////////////



#define arraySize 5 /// ขนาดของข้อมูลที่ใช้ส่งข้อมูลระหว่างบอร์ด
#define active_button 12 /// pin ที่ต่อปุ่ม pull down เพื่อใช้สั่งให้บอร์ดทำงาน แต่ล่าสุดไม่ใช้แล้ว ลบไปก็ได้
#define resetA9g_pin 8 /// pin ที่เอาไว้ใช้ reset hardware บอร์ด a9g ปัจจุบันไม่ใช้แล้ว ใช้ reset software แทน ลบไปก็ได้

unsigned int pm2_5 = 0;
unsigned int pm10 = 0;

unsigned int limitOf_sentDataTo_cloud = 0; //limit at 5

unsigned long startTime_uno = 0;
unsigned long timeoutDuration_uno = 15000; // 5000 milliseconds = 5 seconds

unsigned long startTime_a9g = 0;
unsigned long startTime_a9g_restart = 0;
unsigned long timeoutDuration_a9g = 10000; // 5000 milliseconds = 5 seconds

float rx_msg[5];//idx 0:avg_co2 , 1:avg_voc , 2:pm2_5 , 3:pm10
bool receiving_data = false;
bool system_run = true;
bool detect_sendData_error = false;
String check_message_error = "";

//int countTofullArray = 0;
//String resultToUno[arraySize];

int co2Offset = 100; // Offset value (adjust as needed)
int co2Span = 200;   // Span value (adjust as needed)

int sizeOfarray;

String tx_msg_a9g = "";
String rx_msg_a9g = "";
//bool socket_a9g = false;


void setup() {
  pinMode(sig_to_runUno,OUTPUT);
  digitalWrite(sig_to_runUno,HIGH);
  pinMode(carbon_pin,INPUT);
  pinMode(voc_pin,INPUT);
  pinMode(active_button,INPUT);
  
  
  Serial.begin(115200);
  Serial1.begin(115200);//uno rx,tx on pin 19,18
  Serial2.begin(9600);//pm sensor rx,tx on pin 17,16
  Serial3.begin(115200);//a9g module

  
  calibrateMG811();/////set up cabon module



  //////////set up a9g Module/////////
  Serial.println("Wait For Response a9g Module");
  delay(10000);
  check_network_a9g();
  Init_a9g();
  
  Serial1.flush();
  pinMode(resetA9g_pin,OUTPUT);
  digitalWrite(resetA9g_pin,HIGH);

  //////////End set up a9g Module/////////


  ////////////// set up SEN55-SDN-T/////////////////

       Wire.begin();

        sen5x.begin(Wire);
    
        uint16_t error;
        char errorMessage[256];
        error = sen5x.deviceReset();
        if (error) {
            Serial.print("Error trying to execute deviceReset(): ");
            errorToString(error, errorMessage, 256);
            Serial.println(errorMessage);
        }
    
    // Print SEN55 module information if i2c buffers are large enough
        #ifdef USE_PRODUCT_INFO
            printSerialNumber();
            printModuleVersions();
        #endif
    
        
        float tempOffset = 0.0;
        error = sen5x.setTemperatureOffsetSimple(tempOffset);
        if (error) {
            Serial.print("Error trying to execute setTemperatureOffsetSimple(): ");
            errorToString(error, errorMessage, 256);
            Serial.println(errorMessage);
        } else {
            Serial.print("Temperature Offset set to ");
            Serial.print(tempOffset);
            Serial.println(" deg. Celsius (SEN54/SEN55 only");
        }
    
        // Start Measurement
        error = sen5x.startMeasurement();
        if (error) {
            Serial.print("Error trying to execute startMeasurement(): ");
            errorToString(error, errorMessage, 256);
            Serial.println(errorMessage);
        }

  
  ////////////// End set up SEN55-SDN-T/////////////

  
  pinMode(led_status1,OUTPUT);
  pinMode(led_status2,OUTPUT);
  
  digitalWrite(led_status1,LOW);
  digitalWrite(led_status2,LOW);
}

void loop() {

//  int inp = digitalRead(active_button);
//  if(inp == HIGH){
//    Serial.println("system is running");
//    Serial.println(limitOf_sentDataTo_cloud);
//    system_run = true;
//  }


  if(limitOf_sentDataTo_cloud >= 10 || detect_sendData_error == true){//check limit of data
    Serial.println("Restart a9g board for Stability");
//    digitalWrite(resetA9g_pin,LOW);//}
//    delay(500);                     //}  reset a9g module board by hardware
//    digitalWrite(resetA9g_pin,HIGH);//}
//    socket_a9g = true;
    Serial3.println("AT+RST=1\r");
    delay(4000);
    UpdateSerial_restart();
//    UpdateSerial_a9g();
    delay(10000);
    check_network_a9g();
    Init_a9g();
    limitOf_sentDataTo_cloud = 0;
    detect_sendData_error = false;
  }



  
  if(system_run == true){
    digitalWrite(led_status1,HIGH);
    digitalWrite(led_status2,LOW);
    Serial.println(limitOf_sentDataTo_cloud);
      float avg_co2 = 0;
      float avg_voc = 0;
    
      int index_pm = 0;
      char value_pm;
      char previousValue_pm;
      
      float noX = 0;
    
    
      
      int co2Value = analogRead(carbon_pin);
      avg_co2 = map(co2Value, 0, 1023, co2Offset, co2Span);
    
    
      
      for(int count_round2=0;count_round2<10;count_round2++){
        float raw_data_voc = analogRead(voc_pin);
        avg_voc += raw_data_voc;
        delay(500);
        if(count_round2 == 9){
          avg_voc = avg_voc/10;
    
          }
      }
    



    uint16_t error;
    char errorMessage[256];

    delay(1000);

    // Read Measurement
    float massConcentrationPm1p0;
    float massConcentrationPm2p5;
    float massConcentrationPm4p0;
    float massConcentrationPm10p0;
    float ambientHumidity;
    float ambientTemperature;
    float vocIndex;
    float noxIndex;

    error = sen5x.readMeasuredValues(
        massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
        massConcentrationPm10p0, ambientHumidity, ambientTemperature, vocIndex,
        noxIndex);

    if (error) {
        Serial.print("Error trying to execute readMeasuredValues(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        
        pm2_5 = massConcentrationPm2p5;
        pm10 = massConcentrationPm1p0;
        
        if (isnan(noxIndex)) {
            Serial.println("n/a");
        } else {
            
            noX = noxIndex;
        }
    }




    
      Serial.print("Carbon: ");
      Serial.println(avg_co2);
      
      Serial.print("Voc: ");
      Serial.println(avg_voc);
    
      Serial.print("pm2_5: ");
      Serial.print(pm2_5);
      Serial.println(" ug/m3");
    
      Serial.print("pm10: ");
      Serial.print(pm10);
      Serial.println(" ug/m3");


      Serial1.flush();
      digitalWrite(led_status1,LOW);
      digitalWrite(led_status2,HIGH);
      digitalWrite(sig_to_runUno,LOW);//cmd to run another Board
      delay(1000);
      digitalWrite(sig_to_runUno,HIGH);
      Serial.println("Wait data from another board");
      delay(10000);
      
      startTime_uno = millis();
      while(millis() - startTime_uno < timeoutDuration_uno){
        if(Serial1.available()>0){
          String receivedData = Serial1.readStringUntil('\n');
          splitStringToArray(receivedData);
          break;
        }
      }
      
      displayArray();

      if(sizeOfarray == arraySize){

      digitalWrite(led_status1,HIGH);
      digitalWrite(led_status2,HIGH);
      
      tx_msg_a9g = "AT+HTTPGET=\"https://api.thingspeak.com/update?api_key=3921T8B66R9QH7BR";
      tx_msg_a9g += "&field1=";
      tx_msg_a9g += String(rx_msg[0]);
      tx_msg_a9g += "&field2=";
      tx_msg_a9g += String(pm10);
      tx_msg_a9g += "&field3=";
      tx_msg_a9g += String(pm2_5);
      tx_msg_a9g += "&field4=";
      tx_msg_a9g += String(avg_co2);
      tx_msg_a9g += "&field5=";
      tx_msg_a9g += String(avg_voc);
      tx_msg_a9g += "\"\r";
      
      Serial3.println(tx_msg_a9g);
      Serial.println(tx_msg_a9g);
      delay(10000);
//      socket_a9g = true;
      UpdateSerial_http();
//      system_run = false;
      limitOf_sentDataTo_cloud += 1;
      }
      else{Serial.println(sizeOfarray);}
      Serial.println("Please wait for 60 sec");
      digitalWrite(led_status1,LOW);
      digitalWrite(led_status2,LOW);
      delay(60000);
      
    system_run = true;
  
  }
  
}



void displayArray() {
  Serial.println("Contents of the array:");
//  Serial.println(sizeof(rx_msg));
  for (int i = 0; i < sizeOfarray; i++) {
      Serial.println(rx_msg[i]);
  }
}


void splitStringToArray(String input) {
  int index = 0; // Index for the dataArray
  int spaceIndex = 0; // Variable to store the position of the space character
//  Serial.println(input.substring(0, 6));
 
  while (spaceIndex != -1) {
    spaceIndex = input.indexOf(","); // Find the position of the space character
    
    if(spaceIndex != -1){
      rx_msg[index] = input.substring(0, spaceIndex).toFloat(); // Convert to float and add to array
      input = input.substring(spaceIndex + 1); // Update input string to exclude the processed part
      index++;
    }
  }
  // Add the last element to the array
  rx_msg[index] = input.toFloat();
//  receiving_data = false;
  sizeOfarray = index+1;

}


void calibrateMG811() {
  // Calibration routine
  // Make sure the sensor is in a clean air environment during calibration

  int sum = 0;
  int numSamples = 100;

  for (int i = 0; i < numSamples; i++) {
    sum += analogRead(carbon_pin);
    delay(20);
  }

  co2Offset = sum / numSamples;
  co2Span = co2Offset + 400;  // Adjust span value as needed
}

void Init_a9g(){
  
  Serial3.println("AT\r");
  delay(1000);
//  socket_a9g = true;
  UpdateSerial_a9g();

  Serial3.println("AT+CGATT=1\r");
  delay(1000);
//  socket_a9g = true;
  UpdateSerial_a9g();

  Serial3.println("AT+CGDCONT=1,\"IP\",\"TRUE-H INTERNET\"");
  delay(1000);
//  socket_a9g = true;
  UpdateSerial_a9g();

  Serial3.println("AT+CGACT=1,1");
  delay(1000);
//  socket_a9g = true;
  UpdateSerial_a9g();

  Serial3.println("AT+CIFSR");
  delay(1000);
//  socket_a9g = true;
  UpdateSerial_a9g();
}

//void UpdateSerial_a9g()
//{
////  startTime_a9g = millis();
////  while(millis() - startTime_a9g < timeoutDuration_a9g){
//  //while(socket_a9g == true){
//   while (Serial3.available()>0){  
//    char val = Serial3.read();
////    rx_msg_a9g += val;
//    if(val == '\n'){
//      Serial.println(rx_msg_a9g);
////      Serial.flush();
//      rx_msg_a9g = "";
////      socket_a9g = false;
////      break;
//    }
//    else{
//      rx_msg_a9g += val;
//      
//    }
//    }
////   while (Serial.available())   Serial3.write(Serial.read());
//  //}
//}

void UpdateSerial_a9g()
{
  while(Serial3.available()!=0)
  Serial.write(Serial3.read());
//  delay(5000); 
  
}

void check_network_a9g(){
  bool connect_to_network = false;
  String receive_message = "";
  
  while(connect_to_network == false){
    Serial3.println("AT+CREG?\r");
    delay(1000);
    while(Serial3.available()>0){
      char val = Serial3.read();
  
      if(val == '\n'){
        Serial.println(receive_message);
       if (receive_message.indexOf("+CREG: 1,1") == false) {
          connect_to_network = true;
          break;
        }
        receive_message = "";
      }
      else{
        receive_message += val;
      }
    }
  }
  
}


void UpdateSerial_restart(){
  //  bool socket_a9g = true;
  startTime_a9g_restart = millis();
  while(millis() - startTime_a9g_restart < timeoutDuration_a9g){
//  while(socket_a9g == true){
   while(Serial3.available()!=0){
  Serial.write(Serial3.read());
  }

  }

}

void UpdateSerial_http()
{
//  bool socket_a9g = true;
  startTime_a9g = millis();
  while(millis() - startTime_a9g < timeoutDuration_a9g){
//  while(socket_a9g == true){
    while(Serial3.available()!=0){
      char val = Serial3.read();
      Serial.write(val);
      check_message_error += val;
    }
  }
  
//  if (check_message_error.indexOf("+CME ERROR: 53") == false || 
//   check_message_error.indexOf("failure, pelase check your network or certificate!") == false || 
//   check_message_error.indexOf("HTTP/1.1  500  Internal Server Error") == false || 
//   check_message_error == "") {
//       detect_sendData_error = true;
//   }
    if (check_message_error.indexOf("HTTP/1.1  200  OK") == -1) {
        detect_sendData_error = true;

      }
    check_message_error = "";
}



////////////////Function of SEN55_SDN_T module////////////////


void printModuleVersions() {
    uint16_t error;
    char errorMessage[256];

    unsigned char productName[32];
    uint8_t productNameSize = 32;

    error = sen5x.getProductName(productName, productNameSize);

    if (error) {
        Serial.print("Error trying to execute getProductName(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("ProductName:");
        Serial.println((char*)productName);
    }

    uint8_t firmwareMajor;
    uint8_t firmwareMinor;
    bool firmwareDebug;
    uint8_t hardwareMajor;
    uint8_t hardwareMinor;
    uint8_t protocolMajor;
    uint8_t protocolMinor;

    error = sen5x.getVersion(firmwareMajor, firmwareMinor, firmwareDebug,
                             hardwareMajor, hardwareMinor, protocolMajor,
                             protocolMinor);
    if (error) {
        Serial.print("Error trying to execute getVersion(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("Firmware: ");
        Serial.print(firmwareMajor);
        Serial.print(".");
        Serial.print(firmwareMinor);
        Serial.print(", ");

        Serial.print("Hardware: ");
        Serial.print(hardwareMajor);
        Serial.print(".");
        Serial.println(hardwareMinor);
    }
}


void printSerialNumber() {
    uint16_t error;
    char errorMessage[256];
    unsigned char serialNumber[32];
    uint8_t serialNumberSize = 32;

    error = sen5x.getSerialNumber(serialNumber, serialNumberSize);
    if (error) {
        Serial.print("Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("SerialNumber:");
        Serial.println((char*)serialNumber);
    }
}
