#include<SoftwareSerial.h>
SoftwareSerial client(8,9); //RX, TX//
//#define NUMFLAKES 10
//#define XPOS 0
//#define YPOS 1
//#define DELTAY 2

String webpage="";
boolean OK=false;
boolean Error=false;
///////////////////////////////SETUP//////////////////////////
void setup() 
{
  
   Serial.begin(115200);
   client.begin(115200);


wifi("AT",1000);
wifi("AT+CWMODE=3",1000);
wifi("AT+RST",1000);
wifi("AT+CIPMUX=1",1000);
wifi("AT+CWJAP=\"UPC0048307\",\"DXAAZYSE\"",1000);
wifi("AT+CIFSR",1000);

   
  
}


void loop() 
{
  int i; 
  i=1;
  wifi("AT+CIPSTART=4,\"TCP\",\"192.168.0.15\",80",1000);
wifi("AT+CIPSEND=4,176",1000);
wifi("POST /add.php HTTP/1.1\r\nHost: 192.168.0.15\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 42\r\n\r\nfirstname=ard&surname=ard&emailaddress=" + i + "11\r\n\r\n",1000);

 
  wifi("AT+CIPCLOSE=4",1000);
delay(5000);


i=i+i
  
}


void sendwebdata(String webPage)
{


    int ii=0;
     while(1)
     {
      unsigned int l=webPage.length();
      //Serial.print("AT+CIPSEND=0,");
      client.print("AT+CIPSEND=0,");
      
      
      Serial.println(l+2);
      client.println(l+1);
      
      delay(50);
      Serial.println(webPage);
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



void wifi(String cmd, int t)
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
