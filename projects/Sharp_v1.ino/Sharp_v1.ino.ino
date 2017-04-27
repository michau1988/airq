#define MIN_VOLTAGE     600 // mv - próg dolnego zakresu napięcia dla braku pyłu
#define VREF           1100 // mv - napięcie referencyjne komparatora
#define PIN_LED           7 // numer pinu ILED
#define PIN_ANALOG        0 // numer pinu AOUT
#define MAX_ITERS        10 // liczba pomiarow do sredniej
 
int ADC_VALUE; // odczytana wartosc A0
int ITER; // numer pomiaru
float VOLTAGE; // wartosc napiecia
float DUST; // wynik
float AVG_DUST; // sredni wynik
 
void setup(void)
{
   analogReference(INTERNAL);
 
  Serial.begin(9600);
 
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
}
 
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
 
void loop(void)
{
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
}
