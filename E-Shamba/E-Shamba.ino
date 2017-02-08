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
#include <LiquidCrystal.h>



/////////// CONSTANTS /////////////

#define DHTPIN 7
#define DHTTYPE DHT11
#define OWNER_TEL "+254710446379"

//////////// GLOBAL VARIABLES ////////////


LiquidCrystal lcd(12, 13, 6, 5, 4, 3);  // Initializing LCD pins
DHT dht(DHTPIN, DHTTYPE);               // Initializing Temperature Sensor Pins
Sim800l Sim800l;                        // Initializing SIM module
bool error;                             // To catch errors
float moistureValue;                    // Amount of soil moisture
int temperature;
int humidity;
long sec = 0;
long inter=200000;                      //InterVal in milliseconds to update data to ThingSpeak
String alertMessage="";


void setup()
{
  
  lcd.begin(16,2);                      
  Serial.begin(9600);
  Sim800l.begin();
  dht.begin();
  
  delay(1000);
  Serial.println("Ready");
  lcd.print("Setup Complete!");

  error=Sim800l.delAllSms();                         // Delete all SMS to preVent errors
  if(error) Serial.println("Deleted all sms");
  else Serial.println("No Messages to delete");
  
}

void loop()
{


 ////////// READ SENSOR VALUES ////////////

 
 
 moistureValue = analogRead(0)/100.0;              // Get moisture amount from sensor
 temperature = dht.readTemperature();              // Read temperature from sensor
 humidity = dht.readHumidity();                    // Read humidity from sensor

 

String conditions="Temp"+(String)temperature+"\t humidity"+(String)humidity+"\t Soil Moisture"+(String)moistureValue;
Serial.println(conditions);





////////////// SMS ALERT USER /////////////////



if(temperature > 35.0 || temperature < 18.0){
      alertMessage="ALERT \n The temperature is:"+(String) temperature+". \n Please regulate before damage to crops";
     if(alertUser(string2char(alertMessage))){
      lcd.clear();
     lcd.print("Alerted User");
     }
  }

if(humidity > 50.0 || humidity < 20.0){
   alertMessage="ALERT \n The humidity is:"+(String) humidity+". \n Please regulate before damage to crops";
     if(alertUser(string2char(alertMessage))){
      lcd.clear();
     lcd.print("Alerted User");
     }
}

if(moistureValue > 9.0 || moistureValue < 4.0){
   alertMessage="ALERT \n The Soil Moisture content is:"+(String)moistureValue +".\n Please regulate before damage to crops";
     if(alertUser(string2char(alertMessage))){
      lcd.clear();
     lcd.print("Alerted User");
     }
}

 ////////////////////// Cloud //////////////////////

 
 
  if(millis()-sec>=inter){
    lcd.clear();
    lcd.print("Uploading data...");
    uploadData(temperature,humidity,moistureValue);
    sec = millis();
    lcd.clear();
    lcd.print("Success!");
    delay(2000);
  }
  
  
  
  //////////////////////  LCD ///////////////////////
  
    
  

 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" T    ");
  lcd.print("H   "); 
  lcd.print(" S");
  
  lcd.setCursor(0,1);
  lcd.print(temperature);
  lcd.print("C  ");
  lcd.print(humidity);
  lcd.print("%  ");
  lcd.print(moistureValue);


 
  
 
 delay(10000);
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
  bool res=Sim800l.sendSms(OWNER_TEL,message);
  return res;
  }

