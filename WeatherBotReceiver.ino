#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <TimeLib.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
int send_temperature=20;
int send_humidity=30;
int send_atmospheric=1000;
int send_water_d=1000;
int send_water_a=1000;
int send_dust;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
int extemp=24;
LiquidCrystal_I2C lcd(0x27,16 ,2);
RF24 radio(7, 8); // C, CSN
 
const byte address[6] = "00001";
 
void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
 
void loop() {
  if (radio.available()) {
    radio.read(&send_temperature,sizeof(send_temperature));
    Serial.print("temp:");
    Serial.println(send_temperature);
  }
  if(radio.available()){
    radio.read(&send_humidity,sizeof(send_humidity));
    Serial.print("humidity:");
    Serial.println(send_humidity);
  }
  if(radio.available()){
    radio.read(&send_atmospheric,sizeof(send_atmospheric));
    Serial.print("pressure:");
    Serial.println(send_atmospheric);
  }
  if(radio.available()){
    radio.read(&lowpulseoccupancy,sizeof(lowpulseoccupancy));
    Serial.print("lpo");
    Serial.println(lowpulseoccupancy);
  }
  if(radio.available()){
    radio.read(&ratio,sizeof(ratio));
    Serial.print("ratio:");
    Serial.println(ratio);
  }
  if(radio.available()){
    radio.read(&concentration,sizeof(concentration));
    Serial.print("concentration:");
    Serial.println(concentration);
  }
  if(radio.available() ){
    radio.read(&send_water_d,sizeof(send_water_d));
    send_water_d-=10;
    Serial.print("water_d:");
    Serial.println(send_water_d);
  }
  if(radio.available()){
    radio.read(&send_water_a,sizeof(extemp*2));
    send_water_a-=10;
    Serial.print("water_a:");
    Serial.println(send_water_a);
  }
  Serial.println("--------------------");
  delay(10000);
}
