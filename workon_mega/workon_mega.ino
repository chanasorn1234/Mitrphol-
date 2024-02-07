
#define carbon_pin  A1
#define voc_pin  A2
#define arraySize 5
unsigned int pm2_5 = 0;
unsigned int pm10 = 0;

int countTofullArray = 0;
String resultToUno[arraySize];
  
void setup() {
  pinMode(carbon_pin,INPUT);
  pinMode(voc_pin,INPUT);
  Serial.begin(9600);
  Serial1.begin(9600);//uno rx,tx on pin 19,18
  Serial2.begin(9600);//pm sensor rx,tx on pin 17,16

}

void loop() {
  float avg_co2 = 0;
  float avg_voc = 0;

  int index_pm = 0;
  char value_pm;
  char previousValue_pm;

  countTofullArray = 0;
  
  for(int count_round=0;count_round<10;count_round++){ 
    float raw_data_carbon = analogRead(carbon_pin);
    avg_co2 += raw_data_carbon;
    delay(500);
    if(count_round == 9){
      avg_co2 = avg_co2/10;
      resultToUno[countTofullArray]=avg_co2;
      countTofullArray++;
      }
  }
  for(int count_round2=0;count_round2<10;count_round2++){
    float raw_data_voc = analogRead(voc_pin);
    avg_voc += raw_data_voc;
    delay(500);
    if(count_round2 == 9){
      avg_voc = avg_voc/10;
      resultToUno[countTofullArray]=avg_voc;
      countTofullArray++;
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
    resultToUno[countTofullArray]=pm2_5;
    countTofullArray++;

  }
  else if (index_pm == 9) {
    pm10 = 256 * previousValue_pm + value_pm;
    resultToUno[countTofullArray]=pm10;
    countTofullArray++;

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

  resultToUno[countTofullArray]=2.56;//for sulfur
  countTofullArray++;

  
  Serial.println(countTofullArray);
  String combinedResult = "";
  if(countTofullArray == arraySize){
    for (int i = 0; i < arraySize; i++) {
      combinedResult += resultToUno[i];
      if (i < arraySize - 1) {
        combinedResult += ","; // Add a space between strings
      }
    }
  
    // Send the combined string through SoftwareSerial
    Serial.println(combinedResult);
    Serial1.print(combinedResult);
  }
//  Serial1.print(avg_co2);
//  Serial1.println();
//  Serial1.print(avg_voc);
//  Serial1.println();
//  Serial1.print(pm2_5);
//  Serial1.println();
//  Serial1.print(pm10);
//  Serial1.println();
//  Serial1.print(avg_voc);
  
}
