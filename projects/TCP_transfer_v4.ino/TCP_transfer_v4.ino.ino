#include<SoftwareSerial.h>
SoftwareSerial client(8,9); //RX, TX//
//#define NUMFLAKES 10
//#define XPOS 0
//#define YPOS 1
//#define DELTAY 2

int ic;
String part1 = "POST /add.php HTTP/1.1\r\nHost: 192.168.0.15\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 42\r\n\r\nfirstname=ard&surname=ard&emailaddress=";
String part2 = "--\r\n\r\n";




 
   
String webpage="";
boolean OK=false;
boolean Error=false;
///////////////////////////////SETUP//////////////////////////
void setup() 
{
  
   Serial.begin(115200);
   client.begin(115200);

wifi("AT",100);
  wifi("AT+CWMODE=3",100);
  wifi("AT+RST",100);
  wifi("AT+CIPMUX=1",200);
  //rozwazyc rozlaczenie z poprzednia siecia
  wifi("AT+CWJAP=\"UPC0048307\",\"DXAAZYSE\"",4000);
  wifi("AT+CIFSR",2000);


  ic=1;

Serial.println("-------------------Probably connected :) ----------------\r\n\r\n");

}


void loop() 
{
String ii = String(ic);  
  
  wifi("AT+CIPSTART=4,\"TCP\",\"192.168.0.15\",80",500);
  client.println("AT+CIPSEND=4,176");
  delay(100);
  wifi(part1 + ii + part2,2000);
  wifi("AT+CIPCLOSE=4",500);


ic=ic+1;
  
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
    OK=true;
  }
  else
  {
    Serial.print("Error - ");
    Serial.println(cmd);
    Error=true;
  }
}
