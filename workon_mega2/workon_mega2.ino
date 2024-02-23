
#define carbon_pin  A1
#define voc_pin  A2
#define arraySize 5
unsigned int pm2_5 = 0;
unsigned int pm10 = 0;

float rx_msg[5];//idx 0:avg_co2 , 1:avg_voc , 2:pm2_5 , 3:pm10
bool receiving_data = false;

//int countTofullArray = 0;
//String resultToUno[arraySize];

int co2Offset = 100; // Offset value (adjust as needed)
int co2Span = 200;   // Span value (adjust as needed)

int sizeOfarray;
void setup() {
  pinMode(carbon_pin,INPUT);
  pinMode(voc_pin,INPUT);
  Serial.begin(115200);
  Serial1.begin(115200);//uno rx,tx on pin 19,18
  Serial2.begin(9600);//pm sensor rx,tx on pin 17,16

  calibrateMG811();

}

void loop() {
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



  

  receiving_data = true;
  while(receiving_data == true){
    while(Serial1.available()>0){
      String receivedData = Serial1.readStringUntil('\n');
//      Serial.println(receivedData);
      splitStringToArray(receivedData);

    }
  }
  displayArray();
  
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
  receiving_data = false;
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
