//sharp
#define MIN_VOLTAGE     600 // mv - próg dolnego zakresu napięcia dla braku pyłu
#define VREF           1100 // mv - napięcie referencyjne komparatora
#define PIN_LED           12 // numer pinu ILED
#define PIN_ANALOG        0 // numer pinu AOUT
#define MAX_ITERS        10 // liczba pomiarow do sredniej
int ADC_VALUE; // odczytana wartosc A0
int ITER; // numer pomiaru
float VOLTAGE; // wartosc napiecia
float DUST; // wynik
float AVG_DUST; // sredni wynik
//PIN 12 - LED pomiarowy
//PIN A0 - odczyt napiecia


//Liberies
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

//Wifi
SoftwareSerial client(8,9); //RX, TX

//LCD
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

// Temperatura PIN 2 on the Arduino
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//Zmienne
int i=0;//k=0;
String readString;
//int x=0;
boolean No_IP=false;
String IP="";
//char temp1='0';
int ic;



void get_ip()
{
  IP="";
  char ch="0";
  while(1)
  {
    client.println("AT+CIFSR");
    
    Serial.println("CIFSR sent");
    
    while(client.available()>0)
    {
    
      if(client.find("STAIP,\""))
      {
        delay(2000);
        Serial.print("IP Address:");
        while(client.available()>0)
        {
          delay(200);
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
  IP = IP.substring(0,13);
  IP.replace('/','.');
  Serial.print(IP);
  Serial.print("Port:");
  Serial.println(80);
}


void wifi(String cmd, int t)
{
   int i=0;
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
    if(i>7)
    break;
    i++;
  }
  if(i==8)
  {
    Serial.print("OK - ");
    Serial.println(cmd);
    
  }
  else
  {
    Serial.print("Error - ");
    Serial.println(cmd);
    
  }
}


void wifi_init() 
{
  
  wifi("AT",100);
  wifi("AT+CWMODE=3",100);
  wifi("AT+RST",100);
  wifi("AT+CIPMUX=1",200);
  //rozwazyc rozlaczenie z poprzednia siecia
  wifi("AT+CWJAP=\"UPC242464973\",\"ET2944H4\"",4000);
  wifi("AT+CIFSR",2000);
}


//sharp
float computeDust()
{
  // Blyskamy IR, czekamy 280ms, odczytujemy napiecie ADC
  digitalWrite(PIN_LED, HIGH);
  delayMicroseconds(280);
  ADC_VALUE = analogRead(PIN_ANALOG);
  digitalWrite(PIN_LED, LOW);
  // Przeliczamy na mV. Calosc mnozymy przez 11, poniewaz w module
  // zastosowano dzielinik napiecia 1k/10k
  VOLTAGE = (VREF / 1024.0) * ADC_VALUE * 11;
  // Obliczamy zanieczyszczenie jesli zmierzone napiecie ponad prog
  if (VOLTAGE > MIN_VOLTAGE)
    {
      return (VOLTAGE - MIN_VOLTAGE) * 0.2;
    }
  return 0;
}


//////////////////          SETUP          ///////////////////////
void setup() 
{
///sharp init
  analogReference(INTERNAL);
  Serial.begin(9600);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
//sharp init


  
   Serial.begin(115200);
   client.begin(115200);

//connect Wifi
   wifi_init();

//inicializacja termometru
    sensors.begin();


//inicjalizacja wyswietlacza
 display.display(); // show splashscreen
 delay(20);
 display.clearDisplay();   // clears the screen and buffer
 display.begin();
 display.setContrast(50);
//get_ip(); 
 //tymczasowy licznik
 ic=1; 
Serial.println("------------------- Init done :) ----------------\r\n\r\n");
  
}



///////////////////    LOOP     ///////////////////
void loop() 
{

//sharp
 AVG_DUST = 0;
   ITER = 0;
 
   while (ITER < MAX_ITERS)
   {
     DUST = computeDust();
     // Do sredniej liczmy tylko prawidlowe pomiary
     if (DUST > 0)
     {
       AVG_DUST += DUST;
       ITER++;
       delay(50);
     }     
   }
   
   AVG_DUST /= MAX_ITERS;
   
   Serial.print("D = ");
   Serial.print(AVG_DUST);
   Serial.println("ug/m3");    
 
   delay(500);

   
  
sensors.requestTemperatures();

display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(0,0);
display.clearDisplay();

display.println(IP);
display.print("Temp1: ");
display.println(sensors.getTempCByIndex(0));
display.print("Temp2: ");
display.println(sensors.getTempCByIndex(1));
display.print(AVG_DUST);
display.println(" ug/m3");    
display.display();
delay(2);
display.clearDisplay();  



//Sending to database:
String ii = String(ic);  

  
  wifi("AT+CIPSTART=4,\"TCP\",\"79.137.80.222\",80",200);
  client.println("AT+CIPSEND=4,241"); //257
  delay(100);
  ///////////////////////////////tu ponizej wlasciwy POST docelowo //////////////////////////////////////////////////////////////////////////////////


//String post = "GET /insertdata1.php HTTP/1.1\r\nHost: 79.137.80.222\r\nContent-Type: application/x-www-form-urlencoded\r\n"; //101   
//content lenght!
//String post2 = "Content-Length: 137\r\n\r\n"; //27 //powinno byc 129
//String post3 = "asd=1";
//String id_s = "id_station=1.0";                 //14
//String date = "&date=2020-03-02%2021:21:21";    //27
//String s_v = "&software_version=22";            //20
//String id_c = "&id_configuration=22";           //20
//String d1 = "&d1=";                             //4+5
//String d2 = "&d2=";                             //4+4
//String d3 = "&d3=";                             //4+6
//String d4 = "&d4=100";                          //4+3
//String ending = "      \r\n\r\n";               //14

//129



              client.println("GET /insertdata1.php HTTP/1.1\r\nHost: 79.137.80.222\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 137\r\nid_station=1&date=2020-03-02 21:21:21&software_version=22&id_configuration=22&d1=666&d2=666&d3=666&d4=100\r\n\r\n");
  
  //              client.println(post + post2 + id_s + date + s_v + id_c + d1 + sensors.getTempCByIndex(0) + d2 + sensors.getTempCByIndex(1) + d3 + AVG_DUST + ending);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  Serial.println("Sending to database");
  

  wifi("AT+CIPCLOSE=4",500);
  
//tymczasowy licznik
ic=ic+1;

delay(10000000);
 
 
}


