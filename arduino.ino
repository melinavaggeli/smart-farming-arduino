#include <SoftwareSerial.h> //Software Serial library 
#include <dht.h> 
dht DHT; 
  
#define DHT11_PIN 8 
  
SoftwareSerial espSerial(2, 3);   //Pin 2 and 3 act as RX and TX. Connect them to TX and RX of ESP8266       
  
#define DEBUG true 
  
String mySSID = "iPhone - Efthymia";       // WiFi SSID 
String myPWD = "12345678"; // WiFi Password 
String myAPI = "AQQFIATJ76POGJ76";   // API Key 
String myHOST = "api.thingspeak.com"; 
String myPORT = "80"; 
String myFIELD1 = "field1";  
String myFIELD2 = "field2";  
String myFIELD3 = "field3";  
  
int sendTemp=0; 
int sendHumid=0; 
int sendSoilHumid=0; 
int val = 0; //value for storing moisture value  
int soilPin = A0;//Declare a variable for the soil moisture sensor  
int soilPower = 7;//Variable for Soil moisture Power 
int thresholdUp = 400; 
int thresholdDown = 250; 

void setup() 
{ 
   
Serial.begin(9600); 
  
  pinMode(soilPower, OUTPUT);//Set D7 as an OUTPUT 
  digitalWrite(soilPower, LOW);//Set to LOW so no power is flowing through the sensor 
  espSerial.begin(115200); 
    delay(1000); 
  espData("AT+RST",1000, DEBUG);    
   
//Reset the ESP8266 module 
    delay(1000); 
  
  espData("AT+CWMODE=1", 1000, DEBUG);    
    delay(1000); 
  
//Set the ESP mode as station mode 
  espData("AT+CWJAP=\""+ mySSID +"\",\""+ myPWD +"\"", 1000, DEBUG);   //Connect to WiFi network 
  
 /*(  while(!esp.find("OK"))  

{           
      //Wait for connection 
  }*/ 
  delay(1000); 
   
} 
  void loop() 
  { 
  
    sendSoilHumid= readSoil();  
    int chk = DHT.read11(DHT11_PIN); 
    sendTemp = DHT.temperature; 
    sendHumid = DHT.humidity;    
    
    delay(1000); 
  
     
    String sendData = "GET /update?api_key="+ myAPI +"&"+ myFIELD1 +"="+String(sendTemp); 
    espData("AT+CIPMUX=1", 1000, DEBUG);       //Allow multiple connections 
    espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG); 
    espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);   
    espSerial.find(">");  
    espSerial.println(sendData); 
    Serial.print("Temperature to be sent: "); 
    Serial.println(sendTemp); 
      
    espData("AT+CIPCLOSE=0",1000,DEBUG); 
    delay(10000); 
  
    String sendHData = "GET /update?api_key="+ myAPI +"&"+ myFIELD2 +"="+String(sendHumid); 
    espData("AT+CIPMUX=1", 1000, DEBUG);       //Allow multiple connections 
    espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG); 
    espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);   
    espSerial.find(">");  
    espSerial.println(sendData); 
    Serial.print("Humidity to be sent: "); 
    Serial.println(sendHumid); 
      
    espData("AT+CIPCLOSE=0",1000,DEBUG); 
    delay(10000); 
  
    String sendSoilData = "GET /update?api_key="+ myAPI +"&"+ myFIELD3 +"="+String(sendSoilHumid); 
    espData("AT+CIPMUX=1", 1000, DEBUG);       //Allow multiple connections 
    espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG); 
    espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);   
    espSerial.find(">");  
    espSerial.println(sendData); 
    Serial.print("Soil Humidity to be sent: "); 
    Serial.println(sendSoilHumid); 
   
  //if it needs water or not 
  if (readSoil() <= thresholdDown){ 
    Serial.print("Dry, Water it!"); 
    Serial.println(); 
  }else if (readSoil() >= thresholdUp){ 
    Serial.print("Wet, Leave it! "); 
    Serial.println(); 
     
  } 
      
    espData("AT+CIPCLOSE=0",1000,DEBUG); 
    delay(10000); 
     
  } 
  String espData(String command, const int timeout, boolean debug) 
{ 
  
  Serial.print("AT Command ==> "); 
  Serial.print(command); 
  Serial.println("     "); 
   
  String response = ""; 
  espSerial.println(command); 
  long int time = millis(); 
   
while ( (time + timeout) > millis()) 
  { 
    while (espSerial.available()) 
    { 
      char c = espSerial.read(); 
      response += c; 
    } 
  } 
  if (debug) 
  { 
    //Serial.print(response); 
  } 
  return response; 
} 
  
int readSoil() 
{ 
    digitalWrite(soilPower, HIGH);//turn D7 "On" 
    delay(10);//wait 10 milliseconds  
    val = analogRead(soilPin);//Read the SIG value form sensor  
    digitalWrite(soilPower, LOW);//turn D7 "Off" 
    return val;//send current moisture value 
} 
