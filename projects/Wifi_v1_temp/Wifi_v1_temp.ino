#include<SoftwareSerial.h>

#include <OneWire.h>
#include <DallasTemperature.h>
SoftwareSerial client(2,3); //RX, TX 
#define ONE_WIRE_BUS 13
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

boolean OK=false;
boolean Error=false;
String IP="";
String webpage="";



//////////////////////GET IP//////////////////////
void get_ip()
{
  Serial.print("Obtain IP Address:");
  IP="";
  char ch=0;
  while(1)
  
  {
    Serial.print("While\r\n");
    client.println("AT+CIPSTA=\"192.168.0.103\"");
    delay(5000);
    client.println("AT+CIFSR");
    delay(5000);
    while(client.available()>0)
    
    {
      Serial.print("Available\r\n");
      if(client.find("STAIP,"))
     
      {
         
        Serial.print("IF Static IP");
        delay(1000);
        Serial.print("IP Address:");
        while(client.available()>0)
        {
          ch=client.read();
          if(ch=='+')
          break;
          IP+=ch;
        }
      }
      if(ch=='+')
      break;
    }
    if(ch=='+')
    break;
    delay(1000);
  }
  Serial.print(IP);
  Serial.print("Port:");
  Serial.println(80);
}

///////////////////////////////////////////////////

/////////////////////CONECT WIFI///////////////////
void connect_wifi(String cmd, int t)
{
  int temp=0,i=0;
  while(1)
  {
    Serial.println(cmd);
    client.println(cmd); 
    while(client.available())
    {
      if(client.find("OK"))
      i=8;
    }
    delay(t);
    if(i>5)
    break;
    i++;
  }
  if(i==8)
  {
    Serial.println("OK");
    OK=true;
  }
  else
  {
    Serial.println("Error");
    Error=true;
  }
}
/////////////////////////////////////////////////////

//////////////////////WIFI INIT/////////////////////
void wifi_init()
{
      connect_wifi("AT",100);
      OK=false;
      connect_wifi("AT+CWMODE=3",100);
      OK=false;
      connect_wifi("AT+CWQAP",100);  
      OK=false;
      connect_wifi("AT+RST",5000);
      OK=false;
           
        Serial.println("Connecting Wifi....");
        connect_wifi("AT+CWJAP=\"Asd\",\"1234567888\"",7000);         //provide your WiFi username and password here
        
        if(OK=true)
          {
            Serial.println("Wifi Connected"); 
            //get_ip();
            connect_wifi("AT+CIPSTA=\"192.168.0.103\"",2000);
            connect_wifi("AT+CIPMUX=1",100);
            connect_wifi("AT+CIPSERVER=1,80",100);
          }
          else
          {
          }
          Serial.println("Master Error"); 
}

////////////////////////////////////////////////////

///////////////////////////SENDING///////////////////////

void sendwebdata(String webPage)
{
    int ii=0;
     while(1)
     {
      unsigned int l=webPage.length();
      Serial.print("AT+CIPSEND=0,");
      client.print("AT+CIPSEND=0,");
      Serial.println(l+2);
      client.println(l+2);
      delay(100);
      Serial.println(webPage);
      client.println(webPage);
      while(client.available())
      {
        //Serial.print(Serial.read());
        if(client.find("OK"))
        {
          ii=11;
          break;
        }
      }
      if(ii==11)
      break;
      delay(100);
     }
}

//////////////////////////////////////////////////////////////////////////

void setup() 
{
   Serial.begin(115200);
   client.begin(115200);
   wifi_init();
   Serial.println("System Ready..");
   sensors.begin();
}

void loop() 
{
  sensors.requestTemperatures();
  float temp0=(sensors.getTempCByIndex(0));
  float temp1=(sensors.getTempCByIndex(1));
 delay(500);
webpage = temp0 ;
webpage+=" ";
webpage+=temp1;
sendwebdata(webpage);


client.println("AT+CIPCLOSE=0"); 

 
 
  
}












