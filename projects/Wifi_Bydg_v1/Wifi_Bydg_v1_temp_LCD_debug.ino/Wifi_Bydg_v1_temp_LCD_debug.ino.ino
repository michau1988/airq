#include<SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

SoftwareSerial client(8,9); //RX, TX
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

String webpage="";
int i=0,k=0;
String readString;
int x=0;
boolean No_IP=false;
String IP="";
char temp1='0';

     
void check4IP(int t1)
{
  int t2=millis();
  while(t2+t1>millis())
  {
    while(client.available()>0)
    {
      if(client.find("WIFI GOT IP"))
      {
        No_IP=true;
      }
    }
  }
}
void get_ip()
{
  IP="";
  char ch=0;
  while(1)
  {
    client.println("AT+CIFSR");
    while(client.available()>0)
    {
      if(client.find("STAIP,"))
      {
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
  //Serial.println(80);
}
void connect_wifi(String cmd, int t)
{
  int temp=0,i=0;
  while(1)
  {
    //Serial.println(cmd);
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
  //if(i==8)
  //Serial.println("OK");
  //else
  //Serial.println("Error");
}
void wifi_init()
{
      connect_wifi("AT",100);
      connect_wifi("AT+CWMODE=3",100);
      connect_wifi("AT+CWQAP",100);  
      connect_wifi("AT+RST",5000);
      check4IP(5000);
      if(!No_IP)
      {
        //Serial.println("Connecting Wifi....");
        connect_wifi("AT+CWJAP=\"UPC0048307\",\"DXAAZYSE\"",7000);         //provide your WiFi username and password here
     // connect_wifi("AT+CWJAP=\"vpn address\",\"wireless network\"",7000);
      }
      else
        {
        }
      //Serial.println("Wifi Connected"); 
      get_ip();
      connect_wifi("AT+CIPMUX=1",100);
      connect_wifi("AT+CIPSERVER=1,80",100);
}


////////////////////////SENDING////////////////////
void sendwebdata(String webPage)
{
  display.println("Sending...");

 display.display();

    int ii=0;
     while(1)
     {
      unsigned int l=webPage.length();
      //Serial.print("AT+CIPSEND=0,");
      client.print("AT+CIPSEND=0,");
      
      
      //Serial.println(l+2);
      client.println(l+1);
      
      delay(50);
      //Serial.println(webPage);
      client.println(webPage);
      while(client.available())
      {
        delay(100);
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

///////////////////////////////SETUP//////////////////////////
void setup() 
{
   Serial.begin(115200);
   client.begin(115200);
   wifi_init();
    sensors.begin();

 display.display(); // show splashscreen
  delay(20);
  display.clearDisplay();   // clears the screen and buffer

  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(50);
  

  
}
void loop() 
{
sensors.requestTemperatures();

display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.clearDisplay();
display.println("Waiting");
display.println(IP);
 display.display();
  delay(2);
display.clearDisplay();  
  k=0;
  //Serial.println("Please Refresh your Page");
  while(k<1000)
  {
    k++;
   while(client.available())
   {
    if(client.find("0,CONNECT"))
    {
      
      //Serial.println("Start Printing");
      
      Send();
      //Serial.println("Done Printing");
      delay(1000);
    }
  }
  delay(1);
 }
}
void Send()
{

 webpage = "<html><head></head>";
      sendwebdata(webpage);
      webpage = "<h1>Metro Station Page</h1><body bgcolor=f0f0f0>";
      
      sendwebdata(webpage);
      delay(250);
      webpage = "Temp 1: ";
      webpage+=(sensors.getTempCByIndex(0));
      delay(250);
      webpage = "Temp 2: ";
      webpage+=(sensors.getTempCByIndex(1));
      sendwebdata(webpage);
      webpage = "</body></html>";
      sendwebdata(webpage);
     // display.println("Completed");
     // display.display();
        

      
      
     
     client.println("AT+CIPCLOSE=0"); 
    //delay(500);
}
