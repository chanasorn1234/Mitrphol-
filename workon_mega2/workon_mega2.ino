
#define carbon_pin  A1
#define voc_pin  A2
#define arraySize 5
#define active_button 12
#define sig_to_runUno 7
#define resetA9g_pin 8
#define led_status1 2
#define led_status2 3

unsigned int pm2_5 = 0;
unsigned int pm10 = 0;

unsigned int limitOf_sentDataTo_cloud = 0; //limit at 5

unsigned long startTime_uno = 0;
unsigned long timeoutDuration_uno = 15000; // 5000 milliseconds = 5 seconds

unsigned long startTime_a9g = 0;
unsigned long timeoutDuration_a9g = 5000; // 5000 milliseconds = 5 seconds

float rx_msg[5];//idx 0:avg_co2 , 1:avg_voc , 2:pm2_5 , 3:pm10
bool receiving_data = false;
bool system_run = false;

//int countTofullArray = 0;
//String resultToUno[arraySize];

int co2Offset = 100; // Offset value (adjust as needed)
int co2Span = 200;   // Span value (adjust as needed)

int sizeOfarray;

String tx_msg_a9g = "";
String rx_msg_a9g = "";
//bool socket_a9g = false;


void setup() {
  pinMode(carbon_pin,INPUT);
  pinMode(voc_pin,INPUT);
  pinMode(active_button,INPUT);
  pinMode(sig_to_runUno,OUTPUT);
  digitalWrite(sig_to_runUno,HIGH);
  Serial.begin(115200);
  Serial1.begin(115200);//uno rx,tx on pin 19,18
  Serial2.begin(9600);//pm sensor rx,tx on pin 17,16
  Serial3.begin(115200);//a9g module
  
  calibrateMG811();
  Serial.println("Wait For Response a9g Module");
  delay(10000);
  Init_a9g();
  
  Serial1.flush();
  pinMode(resetA9g_pin,OUTPUT);
  digitalWrite(resetA9g_pin,HIGH);
  
  pinMode(led_status1,OUTPUT);
  pinMode(led_status2,OUTPUT);
  
  digitalWrite(led_status1,LOW);
  digitalWrite(led_status2,LOW);
}

void loop() {

  int inp = digitalRead(active_button);
  if(inp == HIGH){
    Serial.println("system is running");
    Serial.println(limitOf_sentDataTo_cloud);
    system_run = true;
//    limitOf_sentDataTo_cloud = 2;
  }


  if(limitOf_sentDataTo_cloud >= 4){//check limit of data
    Serial.println("Restart a9g board for Stability");
    digitalWrite(resetA9g_pin,LOW);//}
    delay(500);                     //}  reset a9g module board by hardware
    digitalWrite(resetA9g_pin,HIGH);//}
//    socket_a9g = true;
    delay(1000);
    UpdateSerial_a9g();
    delay(10000);
    Init_a9g();
    limitOf_sentDataTo_cloud = 0;
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
    
      while (Serial2.available() || index_pm==0) {
      value_pm = Serial2.read();
      if ((index_pm == 0 && value_pm != 0x42) || (index_pm == 1 && value_pm != 0x4d)){
        Serial.println("Cannot find the data header.");
        index_pm = 0;
        //break;
      }
       
      if (index_pm == 4 || index_pm == 6 || index_pm == 8 || index_pm == 10 || index_pm == 12 || index_pm == 14) {
        previousValue_pm = value_pm;
      }
    
      else if (index_pm == 7) {
        pm2_5 = 256 * previousValue_pm + value_pm;
    
    
      }
      else if (index_pm == 9) {
        pm10 = 256 * previousValue_pm + value_pm;
    
    
      } 
      else if (index_pm > 15) {
        break;
      }
      index_pm++;
    }
      while(Serial2.available()) Serial2.read();
    //  Serial.println(" }");
    
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
      UpdateSerial_a9g();
//      system_run = false;
      limitOf_sentDataTo_cloud += 1;
      }
      else{Serial.println(sizeOfarray);}
      Serial.println("Please wait for 60 sec");
      digitalWrite(led_status1,LOW);
      digitalWrite(led_status2,LOW);
      delay(60000);
      

  
  }
//  system_run = false;
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

void UpdateSerial_a9g()
{
//  startTime_a9g = millis();
//  while(millis() - startTime_a9g < timeoutDuration_a9g){
  //while(socket_a9g == true){
   while (Serial3.available()>0){  
    char val = Serial3.read();
//    rx_msg_a9g += val;
    if(val == '\n'){
      Serial.println(rx_msg_a9g);
//      Serial.flush();
      rx_msg_a9g = "";
//      socket_a9g = false;
//      break;
    }
    else{
      rx_msg_a9g += val;
      
    }
    }
//   while (Serial.available())   Serial3.write(Serial.read());
  //}
}
