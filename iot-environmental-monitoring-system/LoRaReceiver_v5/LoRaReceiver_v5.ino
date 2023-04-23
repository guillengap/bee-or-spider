// AUTHOR: GUILLERMO PEREZ GUILLEN

#include <SPI.h>
#include <LoRa.h>
#include <Arduino.h>   // SERCOM1
#include "wiring_private.h" // SERCOM1

// SERCOM1: Rx->D9 & Tx->D8
Uart Serial3 (&sercom1, 9, 8, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM1_Handler()
{
  Serial3.IrqHandler();
}

#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // Change to (0x27,16,2) for 16x2 LCD.

char cadena[30]; //We create an array that will store the characters that we will write in the PC console. We assign a limit of characters, in this case 30
byte posicion=0;  //Variable to change the position of the characters in the array
int valor;  //Integer Variable

void setup() {
  lcd.init();  // Initiate the LCD:
  lcd.backlight();  
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial3.begin(9600); // SERCOM1
  // Assign pins 8 & 9 SERCOM1 functionality
  pinPeripheral(8, PIO_SERCOM);
  pinPeripheral(9, PIO_SERCOM);
  Serial.println("LoRa Receiver");
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    memset(cadena, 0,sizeof(cadena));//memset deletes the contents of the array "cadena" from position 0 to the end sizeof    
    // received a packet
    Serial.print("Received packet... ");  
    // read packet
    while (LoRa.available()) {
      char dedos= (char)LoRa.read();
      Serial.print(dedos);
      Serial3.print(dedos);      
      cadena[posicion]=dedos;//Read a character from the string "cadena" from "posicion", then read the next character with "posicion++"
      posicion++;
    }
    posicion=0;
    int signal_rx = LoRa.packetRssi();          
    // print RSSI of packet
    Serial.print(" with RSSI ");
    Serial.println(signal_rx);
    int parte1 = getValue(cadena,',',0).toInt();
    int parte2 = getValue(cadena,',',1).toInt();
    int parte3 = getValue(cadena,',',2).toInt();
    Serial.println(parte1);
    delay(100);
    Serial.println(parte2);
    delay(100);
    Serial.println(parte3);
    delay(100);    
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("RSSI =");
    lcd.setCursor(7, 0); 
    lcd.print(signal_rx);
    lcd.setCursor(11, 0); 
    lcd.print("dBm");        
    lcd.setCursor(0, 1); 
    lcd.print("Temp =");
    lcd.setCursor(8, 1); 
    lcd.print(parte1);
    lcd.setCursor(11, 1); 
    lcd.print("C");    
    lcd.setCursor(0, 2); 
    lcd.print("Hum =");
    lcd.setCursor(8, 2); 
    lcd.print(parte2);
    lcd.setCursor(11, 2); 
    lcd.print("%");        
    lcd.setCursor(0, 3); 
    lcd.print("CO2 =");
    lcd.setCursor(8, 3); 
    lcd.print(parte3);
    lcd.setCursor(11, 3); 
    lcd.print("PPM");                       
    digitalWrite(LED_BUILTIN, HIGH);
    delay(2000);
    digitalWrite(LED_BUILTIN, LOW); 
    delay(12000);
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
