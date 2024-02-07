#include <SoftwareSerial.h>

float rx_msg[5];//idx 0:avg_co2 , 1:avg_voc , 2:pm2_5 , 3:pm10
bool receiving_data = false;
int sizeOfarray;
SoftwareSerial mega(2,3);
void setup() {
  Serial.begin(9600);
  mega.begin(9600);

}

void loop() {
  ///add code comand to mega for sampling /// 
  receiving_data = true;

  
  while(receiving_data == true){
    while(mega.available()>0){
      String receivedData = mega.readStringUntil('\n');
//      Serial.println(receivedData);
      splitStringToArray(receivedData);

    }
  }
  displayArray();
  
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

void displayArray() {
  Serial.println("Contents of the array:");
//  Serial.println(sizeof(rx_msg));
  for (int i = 0; i < sizeOfarray; i++) {
      Serial.println(rx_msg[i]);
  }
}
