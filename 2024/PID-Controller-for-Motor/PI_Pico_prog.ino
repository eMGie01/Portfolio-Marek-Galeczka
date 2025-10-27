#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <SPI.h>

// Wyświetlacz
#define TFT_CS   4
#define TFT_RST  2 
#define TFT_DC   3
#define TFT_MOSI 6  // Data out
#define TFT_SCLK 7  // Clock out

// Enkoder
#define SygA 16 // zielony
#define SygB 17 // niebieski

// silnik
#define ENB 14 // źółty
#define IN4 15 // szary
#define IN3 13 // brązowy

// przyciski
#define przycisk 0 // zielony
#define przycisk2 1 // niebieski


// zmienne globalne
int dir = 0;
int licznik = 0;
int licznik2 = 0;
long T_poprzednie = 0;
int pozycja_poprzednia = 0;
float calka_e = 0;
float u = 0;

// dla metody 2
volatile int pozycja_i = 0;
volatile float V_i = 0;
volatile long T_poprzednie_i = 0;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void setup(){

  Serial.begin(9600);
  pinMode(SygA, INPUT);
  pinMode(SygB, INPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(przycisk, INPUT_PULLUP);
  pinMode(przycisk2, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(SygA), readEncoder, RISING);

  tft.initR(INITR_BLACKTAB); 
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
}

void loop(){

  int pozycja_aktualna = 0;
  float V_metoda_2 = 0;
  noInterrupts();
  pozycja_aktualna = pozycja_i;
  V_metoda_2 = V_i;
  interrupts();

  long T_aktualne = micros();
  float delta_T =  ((float) (T_aktualne - T_poprzednie))/1.0e6;
  float V_metoda_1 = (pozycja_aktualna - pozycja_poprzednia)/delta_T;
  pozycja_poprzednia = pozycja_aktualna;
  T_poprzednie = T_aktualne;

  // zliczenia/s -> RPM
  float v1 = V_metoda_1/960.0*60.0;

  // prędkość zadana
  float v_zad = 80;
  float ymax = 0.05 * v_zad + v_zad;
  float ymin = -0.05 * v_zad + v_zad;

  // nastawy regulatora
  float kp = 0.8; // 1.1
  float ki = 1.5; //6
  // uchyb
  float e = v_zad - v1;
  calka_e = calka_e + e * delta_T;
  u = kp * e + ki * calka_e;

  // anty-windup
  // w prawo
  if (u >= 255 && dir == -1) {
    u = 255;
    calka_e = 0;
  } else if (u <= 0 && dir == -1) {
    u = 0;
    calka_e = 0;
  } 
  // w lewo
  if (u <= -255 && dir == 1) {
    u = -255;
    calka_e = 0;
  } else if (u >= 0 && dir == 1) {
    u = 0;
    calka_e = 0;
  } 

  // stop
  if (u >= 0 && dir == 0) {
    u = 0;
    calka_e = 0;
}


  int pwr = (int) fabs(u);
  if(pwr > 255)
  {
    pwr = 255;
  }

  Serial.print(v1);
  Serial.print(" ");
  Serial.print(e);
  Serial.print(" ");
  Serial.print(v_zad);
  Serial.print(" ");
  Serial.print(u);
  Serial.print(" ");
  Serial.print(ymax);
  Serial.print(" ");
  Serial.print(ymin);
  Serial.println();

dir = kierunek();

setMotor(dir, pwr, ENB, IN3, IN4);


// Wyświetlanie wartości v1 na wyświetlaczu
tft.fillRect(65, 30, 100, 50, ST77XX_BLACK);
tft.setCursor(30, 30);
tft.print("V = ");
tft.print(v1);
tft.setCursor(30, 50);
tft.print("e = ");
tft.print(e);

delay(50);

}

void setMotor(int dir, int pwmVal, int pwm, int in3, int in4){

  analogWrite(pwm, pwmVal);
  if(dir == 1)
  {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

  }
  else if(dir == -1)
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  else
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
}

void readEncoder()
{
  int b = digitalRead(SygB);
  int i = 0;
  if(b>0)
  {
    i = 1;
  }
  else
  {
    i =  -1;
  }

  pozycja_i = pozycja_i + i;

  long T_aktualne = micros();
  float delta_T = ((float) (T_aktualne - T_poprzednie_i))/1.0e6;
  V_i = i/delta_T;
  T_poprzednie_i = T_aktualne;
}

int kierunek()
{
 if (digitalRead(przycisk) == LOW) 
  {
    licznik = licznik + 1;
    delay(200);
    
    if(licznik % 2 ==1)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      dir = -1;
    }
    else
    {
      digitalWrite(LED_BUILTIN, LOW);
      dir = 0;
      licznik = 0;
    }
  }
   if (digitalRead(przycisk2) == LOW) 
  {
    licznik2 = licznik2 + 1;
    delay(200);
    
    if(licznik2 % 2 ==1)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      dir = 1;
    }
    else
    {
      digitalWrite(LED_BUILTIN, LOW);
      dir = 0;
      licznik2 = 0;
    }
  }
  return dir;
}