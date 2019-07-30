#include <TridentTD_LineNotify.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"
#include <SPI.h>
#include <Ethernet.h>

#define BLYNK_PRINT Serial
#define SOILPIN A0          // Digital pin connected to the Soil sensor
#define LDRPIN D4           // LDR
#define RELAY1 D0           // Water Pumb
#define RELAY2 D1           // Fan
#define RELAY3 D2           // Light
#define DHTPIN D8           // DHT sensor
#define RELAY4 D3           // ???
//#define DHTTYPE DHT11     // DHT 11
#define DHTTYPE DHT22       // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21     // DHT 21 (AM2301)
//#define LINE_TOKEN "xxx"  // line token at https://notify-bot.line.me/en/
#define LINE_TOKEN "xxx"    // line token at https://notify-bot.line.me/en/
char auth[] = "xxx"; // 
char ssid[] = "xxx";
char pass[] = "xxx";
int pinValue = 0;
int state = 0;
int statusSystem = 0;

TridentTD_LineNotify myLINE(LINE_TOKEN);

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void setup()
{
  pinMode(LDRPIN, INPUT);
  pinMode(SOILPIN, INPUT);
  pinMode(RELAY1, INPUT);
  pinMode(RELAY2, INPUT);
  pinMode(RELAY3, INPUT);
  pinMode(RELAY4, INPUT);
  Serial.begin(9600);
  dht.begin();
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  delay(1000);

  if (Blynk.connected()) {
    Blynk.run();
  }
  
  timer.run();
  humitempSensor();
  soilSensor();
  ldrSensor();

  if(pinValue == 0){
    statusSystem = 0;
    Serial.println("Manual Control");
  } else {
    statusSystem = 1;
    Serial.println("Automatic Control");
  }
}

BLYNK_WRITE(V1)
{
  pinValue = param.asInt();
}

void alarm() {
  if (state > 0) {
    myLINE.notify("Alert from you home!!");
    Blynk.notify("Alert from you home!!");
    Serial.println("Alert from you home!!");
    delay(5000);
    state = 0;
  }
}

void humitempSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("% Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.println(F("°F "));

  if(statusSystem == 0)
  {
    if(h >= 85)
    {
      delay(200);
      myLINE.notify("ความชื้นในโรงเรือนสูง!!!");
      Serial.println("Humidity is High!!!");      
    }
  }
  else
  {
    if(h >= 85)
    {
      digitalWrite(RELAY2, 1);    
    }
    else
    {
      digitalWrite(RELAY2, 0);          
    }
  }

  if(statusSystem == 0)
  {
    if(t >= 30)
    {
      delay(200);
      myLINE.notify("อุณหภูมิในโรงเรือนสูง!!!");
      Serial.println("Temperature is High!!!");      
    }
  }
  else
  {
    if(t >= 30)
    {
      digitalWrite(RELAY1, 1);     
    }
    else
    {
      digitalWrite(RELAY1, 0);          
    }
  }
}

void soilSensor()
{
  float soilValue = analogRead(SOILPIN);
  soilValue = 100-(soilValue*100)/1024;
  Blynk.virtualWrite(V8, soilValue);

  if(statusSystem == 0)
  {
    if(soilValue <= 60)
    {
      delay(200);
      myLINE.notify("ความชื้นในดินต่ำ!!!");
      Serial.println("Soil Moisture is Low!!!");      
    }
  }
  else
  {
    if(soilValue <= 60)
    {
      digitalWrite(RELAY1, 1);     
    }
    else
    {
      digitalWrite(RELAY1, 0);
    }
  }
  
  Serial.print(F("Soil Moisture: "));
  Serial.println(soilValue);
}

void ldrSensor()
{
  float ldrValue = analogRead(LDRPIN);
  ldrValue = 100-(ldrValue*100)/1024;
  Blynk.virtualWrite(V7, ldrValue);
    
  Serial.print(F("LDR: "));
  Serial.println(ldrValue);
}
