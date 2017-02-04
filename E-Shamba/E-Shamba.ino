/**This program was written by Fatma Ali.
*  tumz92@gmail.com
*  It collects temperature and humidity data from a DHT11 sensor
*  Also collects Soil Moisture data from a sensor
*  Sends the data to ThingSpeak cloud for storage and analysis
*  Notifies user Via SMS if conditions is not faVorable to crops
*  31st January 2017
*   
**/


#include <Sim800l.h>
#include <SoftwareSerial.h>
#include <DHT.h> 

#define DHTPIN 7
#define DHTTYPE DHT11

//////////// Variables ////////////

DHT dht(DHTPIN, DHTTYPE);
Sim800l Sim800l;
bool error;
float moistureValue;
int temperature;
int humidity;
int sec = 0;
int inter=200000;
String alertMessage="";
const char* ownersNumber="+254710446379";

void setup()
{
  
  
  Serial.begin(9600);
  Sim800l.begin();
  dht.begin();
  delay(1000);
  Serial.println("Ready");
  error=Sim800l.delAllSms();
  if(error) Serial.println("Deleted all sms");
  
}

void loop()
{
 
 
 moistureValue = analogRead(0)/100.0;
 temperature = dht.readTemperature();
 humidity = dht.readHumidity();

String conditions="Temp"+(String)temperature+"\t humidity"+(String)humidity+"\t Soil Moisture"+(String)moistureValue;

Serial.println(conditions);

if(temperature > 35.0 || temperature < 20.0){
      alertMessage="ALERT \n The temperature is:"+(String) temperature+". \n Please regulate before damage to crops";
      alertUser(string2char(alertMessage));
  }

if(humidity > 50.0 || humidity < 20.0){
   alertMessage="ALERT \n The humidity is:"+(String) humidity+". \n Please regulate before damage to crops";
     alertUser(string2char(alertMessage));
}

if(moistureValue > 9.0 || moistureValue < 4.0){
   alertMessage="ALERT \n The Soil Moisture content is:"+(String)moistureValue +".\n Please regulate before damage to crops";
     alertUser(string2char(alertMessage));
}

 ////////////////////// Cloud //////////////////////
  if(millis()-sec>=inter){
    uploadData(temperature,humidity,moistureValue);
    sec = millis();
  }
  
  
  
  //////////////////////  LCD ///////////////////////
  
  /**
 
  lcd.setCursor(0,0);
  lcd.print("temp: ");
  lcd.print(dht.readTemperature()); 
  lcd.print("C");
  
  lcd.setCursor(0,1);
  lcd.print("humidity:");
  lcd.print(dht.readHumidity());
  lcd.print("%");

   **/
  

 delay(120000);
 
  
 
 
}




//////////////////////  METHODS ///////////////////////




char* string2char(String command){
  if(command.length()!=0){
    char* p = new char[command.length()+1];
    strcpy(p, command.c_str());
    return p;
  }
}







void uploadData(float temp,float humidity,float soil){
 
  Serial.println("Uploading data...");
  String response = Sim800l.thingSpeakUpdate("field1",temperature);
  Serial.println(response);
     response = Sim800l.thingSpeakUpdate("field2",humidity);
  Serial.println(response);
    response = Sim800l.thingSpeakUpdate("field3",moistureValue);
  Serial.println(response);
  
  }

bool alertUser(char* message){
  bool res=Sim800l.sendSms(ownersNumber,message);
  return res;
  }

