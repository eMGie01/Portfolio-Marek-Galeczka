#include <SPI.h>
#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <Wire.h>
#include <TFLI2C.h>
// Przyciski
#define trigger_button  25
#define menu_button     26
#define unit_button     33
volatile int trigger_button_state = 0;
volatile int trigger_button_state_max = 1;
volatile int menu_button_state = 0;
volatile int unit_button_state = 0;
volatile float unit = 1;
// Wyświetlacz
#define TFT_CS   5
#define TFT_DC   13
#define TFT_RST  18 
#define TFT_MOSI 23  // linia danych
#define TFT_SCLK 19  // Clock out
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
// TF-Luna
#define SDA 21
#define SCL 22
TFLI2C tflI2C;
int16_t tfDist = 0;
int16_t tfTemp = 0;
int16_t tfFlux = 0;
int16_t  tfAddr = TFL_DEF_ADR;
float Distance1 = 0;
float Distance2 = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();
  delay(100);
  pinMode(trigger_button, INPUT_PULLUP);
  pinMode(menu_button, INPUT_PULLUP);
  pinMode(unit_button, INPUT_PULLUP);
  Serial.println("Communication setup complete.");
  Serial.println("Starting LCD setup.");
  tft.initR(INITR_BLACKTAB); 
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.fillRect(0, 0, 100, 100, ST77XX_BLACK);
  tft.setCursor(2, 5);
  tft.print(" _Mode_ ");
  tft.setCursor(2, 15);
  tft.print(" [Default] | _Area_ ");
  tft.setCursor(2, 25);
  tft.print(" _Unit_ ");
  tft.setCursor(2, 35);
  tft.print(" [cm] | _mm_ | _inch_");
  tft.setCursor(2, 45);
  tft.print(" _Distance_ ");
  tft.setCursor(2, 55);
  tft.print(" ___________ ");
  Serial.println("LCD setup complete.");
  Serial.println("Program starts NOW!");
}
void loop() {
  menuButtonFcn();
  unitButtonFcn();
  buttonState();
  triggerButtonFcn();

  if(Serial.read() == 0xAA)
  {
    if( tflI2C.getData( tfDist, tfAddr))
      {
          Serial.println(tfDist);
          delay(500);
      }
  }
}
void triggerButtonFcn(){
  if(0 == digitalRead(trigger_button)){
    trigger_button_state += 1;
    delay(200);
    if(trigger_button_state_max < trigger_button_state){
      trigger_button_state = 0;
    }
    Serial.print("Trigger button State: ");
    Serial.println(trigger_button_state);
    if(0 == menu_button_state){
      if(1 == trigger_button_state){
        float Distance = tfLunaData();
        tft.fillRect(0, 55, 200, 200, ST77XX_BLACK);
        tft.setCursor(20, 55);
        tft.setTextSize(2);
        tft.print(Distance);
        tft.setTextSize(1);
      }
      else{
        tft.fillRect(0, 55, 200, 200, ST77XX_BLACK);
        tft.setCursor(20, 55);
        tft.print(" ___________ ");
      }
    }
    else if(1 == menu_button_state){
      //
      if(1 == trigger_button_state){
        Distance1 = tfLunaData();
        tft.fillRect(0, 55, 200, 200, ST77XX_BLACK);
        tft.setCursor(20, 55);
        tft.setTextSize(2);
        tft.print(Distance1);
        tft.setTextSize(1);
      }
      else if(2 == trigger_button_state){
        Distance2 = tfLunaData();
        tft.fillRect(0, 55, 200, 200, ST77XX_BLACK);
        tft.setCursor(20, 55);
        tft.setTextSize(2);
        tft.print(Distance1);
        tft.setCursor(20, 75);
        tft.print(Distance2);
        tft.setCursor(5, 100);
        tft.print("A: ");
        tft.print(Distance1*Distance2);
        tft.setTextSize(1);
      }
      else{
        tft.fillRect(0, 55, 200, 200, ST77XX_BLACK);
        tft.setCursor(20, 55);
        tft.print(" ___________ ");
      }
    }
  }
}
void menuButtonFcn(){
  if(0 == digitalRead(menu_button)){
    menu_button_state += 1;
    delay(200);
    if(1 < menu_button_state){
      menu_button_state = 0;
    }
    Serial.print("Menu button State: ");
    Serial.println(menu_button_state);
    if(1 == menu_button_state){
      trigger_button_state_max = 2;
      tft.fillRect(0, 15, 150, 10, ST77XX_BLACK);
      tft.setCursor(2, 15);
      tft.print(" _Default_ | [Area] ");
      tft.fillRect(0, 35, 200, 10, ST77XX_BLACK);
      tft.setCursor(2, 35);
      tft.print(" [cm^2]|_mm^2_|_inch^2_");
      unit_button_state = 0;
    }
    else{
      trigger_button_state_max = 1;
      tft.fillRect(0, 15, 150, 10, ST77XX_BLACK);
      tft.setCursor(2, 15);
      tft.print(" [Default] | _Area_ ");
      tft.fillRect(0, 35, 200, 10, ST77XX_BLACK);
      tft.setCursor(2, 35);
      tft.print(" [cm] | _mm_ | _inch_");
      unit_button_state = 0;
    }
  }
}
void unitButtonFcn(){
  if(0 == digitalRead(unit_button)){
    unit_button_state += 1;
    delay(200);
    if(2 < unit_button_state){
      unit_button_state = 0;
    }
    Serial.print("Unit button State: ");
    Serial.println(unit_button_state);
    if(0 == menu_button_state){
      if(1 == unit_button_state){
        tft.fillRect(0, 35, 200, 10, ST77XX_BLACK);
        tft.setCursor(2, 35);
        tft.print(" _cm_ | [mm] | _inch_");
      }
      else if(2 == unit_button_state){
        tft.fillRect(0, 35, 200, 10, ST77XX_BLACK);
        tft.setCursor(2, 35);
        tft.print(" _cm_ | _mm_ | [inch]");
      }
      else{
        tft.fillRect(0, 35, 200, 10, ST77XX_BLACK);
        tft.setCursor(2, 35);
        tft.print(" [cm] | _mm_ | _inch_");
      }
    }
    else{
      if(1 == unit_button_state){
        tft.fillRect(0, 35, 200, 10, ST77XX_BLACK);
        tft.setCursor(2, 35);
        tft.print(" _cm^2_|[mm^2]|_inch^2_");
      }
      else if(2 == unit_button_state){
        tft.fillRect(0, 35, 200, 10, ST77XX_BLACK);
        tft.setCursor(2, 35);
        tft.print(" _cm^2_|_mm^2_|[inch^2]");
      }
      else{
        tft.fillRect(0, 35, 200, 10, ST77XX_BLACK);
        tft.setCursor(2, 35);
        tft.print(" [cm^2]|_mm^2_|_inch^2_");
      }
    }
  }
}
void buttonState(){
  if(0 == menu_button_state){
    if(0 == unit_button_state){
      unit = 1;
    }
    else if( 1 == unit_button_state){
      unit = 10;
    }
    else{
      unit = 2.54;
    }
  }
  else if(1 == menu_button_state){
    if(0 == unit_button_state){
      unit = 1;
    }
    else if( 1 == unit_button_state){
      unit = 10;
    }
    else{
      unit = 2.54;
    }
  }
}

float tfLunaData(){

  uint16_t Distance = 0;
  for(int i=0; i<11; i++){

    tflI2C.getData( tfDist, tfAddr);
    Distance += tfDist*unit;
    delay(10);
  }
  return float(Distance)/11;
}