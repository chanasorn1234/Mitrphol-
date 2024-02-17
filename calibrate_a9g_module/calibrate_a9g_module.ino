#include <SoftwareSerial.h>
//SoftwareSerial a9g(4,5);
String tx_msg = "";
String rx_msg = "";
void setup() 
{
  Serial.begin(115200);
  Serial3.begin(115200);
//  attachInterrupt(0,UpdateSerial,FALLING);
}


void loop() 
{ 

   while (Serial.available()>0){   
    char val = Serial.read();
    if(val == '\n'){
      tx_msg += '\r';
      Serial3.println(tx_msg);
//      Serial3.flush();
      tx_msg = "";
      delay(500);
    }
    else{
      tx_msg += val;
    }
    }

//    delay(500);
    UpdateSerial();



//    while(Serial3.available())    { Serial.write(Serial3.read());  }
//while (Serial.available())    { Serial3.write(Serial.read());     }
// To Test command api AT+HTTPGET="http://calapi.inadiutorium.cz/api/v0/en/calendars/default"
}

void parseData(String buff)
{
  Serial.println(buff);
}



void UpdateSerial()
{
   while (Serial3.available()>0){  
    char val = Serial3.read();
//    rx_msg += val;
    if(val == '\n'){
      Serial.println(rx_msg);
//      Serial.flush();
      rx_msg = "";
    }
    else{
      rx_msg += val;
    }
    }
//   while (Serial.available())   Serial3.write(Serial.read());
}
