// AUTHOR: GUILLERMO PEREZ GUILLEN

#include <WiFiNINA.h> // THINGSPEAK->
#include "secrets.h"
#include "ThingSpeak.h"

char ssid[] = SECRET_SSID;    //  your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

char cadena[30]; //We create an array that will store the characters that we will write in the PC console. We assign a limit of characters, in this case 30
byte posicion=0;  //Variable to change the position of the characters in the array
int valor;  //Integer Variable

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  // THINGSPEAK-> check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  String fv = WiFi.firmwareVersion();
  if (fv != "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }    
  ThingSpeak.begin(client);  //Initialize ThingSpeak
}
 
void loop()
{
  // THINGSPEAK-> Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }  
  if(Serial1.available()) //Is there data inside the buffer?
  {
    memset(cadena, 0,sizeof(cadena));//memset deletes the contents of the array "cadena" from position 0 to the end sizeof
 
    while(Serial1.available()>0) //As long as there is data in the buffer execute the function
    {
      delay(5); 
      cadena[posicion]=Serial1.read();//Read a character from the string "cadena" from "posicion", then read the next character with "posicion++"
      posicion++;
    }
    posicion=0;    
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int parte1 = getValue(cadena,',',0).toInt();
    int parte2 = getValue(cadena,',',1).toInt();
    int parte3 = getValue(cadena,',',2).toInt();
    Serial.println(parte1);
    delay(100);
    Serial.println(parte2);
    delay(100);
    Serial.println(parte3);
    delay(100);
    // set the fields with the values
    ThingSpeak.setField(1, parte1);
    ThingSpeak.setField(2, parte2);
    ThingSpeak.setField(3, parte3); 
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }    
    delay(15000);        
  } 
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
