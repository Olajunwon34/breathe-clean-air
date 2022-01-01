#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include <dht.h>
#define DHT11_PIN 4
dht DHT;
SFE_BMP180 bmp180;
int pin = 6;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 3000;//sampe 3s ;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
const int rain_D = 5;
const int rain_A = A0;
int send_temperature;
int send_humidity;
int send_atmospheric;
int send_water_d;
int send_water_a;
int send_dust;
 
RF24 radio(7, 8); // CE, CSN
 
const byte address[6] = "00001";
 
void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  bool success = bmp180.begin();
  Serial.begin(9600);
  pinMode(rain_D, INPUT);
  pinMode(rain_A, INPUT);
}
 
void loop() {
 
  char status;
  double T, P;
  bool success = false;
  status = bmp180.startTemperature();
  if (status != 0) {
    delay(1000);
    status = bmp180.getTemperature(T);
    if (status != 0) {
      status = bmp180.startPressure(3);
      if (status != 0) {
        delay(status);
        status = bmp180.getPressure(P, T);
        if (status != 0) {
          send_atmospheric = P;
        }
      }
    }
  }
  int chk = DHT.read11(DHT11_PIN);
  send_temperature = DHT.temperature;
  send_humidity = DHT.humidity;
  if (digitalRead(rain_D) == HIGH) {
    send_water_d = 1;
  }
  else {
    send_water_d = 0;
  }
  int temp = analogRead(rain_A);
  send_water_a = (1023 - temp) / 100;
 
  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
 
   if ((millis()-starttime) > sampletime_ms)//if the sampel time == 30s
    {
        ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
        concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
        lowpulseoccupancy = 0;
        starttime = millis();
    }
 
  radio.write(&send_temperature, sizeof(send_temperature));
  radio.write(&send_humidity, sizeof(send_humidity));
  radio.write(&send_atmospheric, sizeof(send_atmospheric));
  radio.write(&lowpulseoccupancy,sizeof(lowpulseoccupancy));
  radio.write(&ratio,sizeof(ratio));
  radio.write(&concentration,sizeof(concentration));
  radio.write(&send_water_d+10, sizeof(send_water_d));
  radio.write(&send_water_a+10, sizeof(send_water_a));
 
  Serial.print("temp:");
  Serial.println(send_temperature);
  Serial.print("humidity:");
  Serial.println(send_humidity);
  Serial.print("pressure:");
  Serial.println(send_atmospheric);
  Serial.print("lpo:");
  Serial.println(lowpulseoccupancy);
  Serial.print("ratio:");
  Serial.println(ratio);
  Serial.print("concentration:");
  Serial.println(concentration);
  Serial.print("water_d:");
  Serial.println(send_water_d);
  Serial.print("water_a:");
  Serial.println(send_water_a);
 
  Serial.println("--------------------");
  delay(10000);
}
