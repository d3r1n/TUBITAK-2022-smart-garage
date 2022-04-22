#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>

#define sclk 13
#define mosi 11
#define cs   10
#define rst  9
#define dc   8

// Color definitions
#define  BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

Adafruit_SSD1331 display = Adafruit_SSD1331(cs, dc, rst);

bool a1Bos=true;
bool a2Bos=true;
bool a3Bos=true;
bool a4Bos=true;

void setup() {

  display.begin();
  display.fillScreen(BLACK);

  sablonGoster()
  
}

void loop() 
{

    if(a1Bos==true)
    {
      a1Yesil();
    }
    else
    {
      a1Kirmizi();
    }
    if(a2Bos==true)
    {
      a2Yesil();
    }
    else
    {
      a2Kirmizi();
    }
    if(a3Bos==true)
    {
      a3Yesil();
    }
    else
    {
      a3Kirmizi();
    }
    if(a4Bos==true)
    {
      a4Yesil();
    }
    else
    {
      a4Kirmizi();
    }
  
}
void sablonGoster()
{
  display.drawRect(0,0,96,64,WHITE); // KARE
  int xOrta=48;
  for(int i=0;i<=64;i++)
  {
  display.drawPixel(xOrta,i, WHITE)     // X ORTADAN BAŞLAYIP EN ALTA DOĞRU ÇİZER                
  }
  int yOrta=32;                                                                               // 2 FOR DÖNGÜSÜNÜN OLUŞTURDUĞU GÖRÜNTÜ = +
  for(int i=0;i<=96;i++)
  {
  display.drawPixel(i,32, WHITE)     //  Y ORTADAN BAŞLAYIP EN SAĞA DOĞRU ÇİZER
 }
 void a1Yesil()
 {
  
    display.setCursor(20,12);
  display.setTextColor(GREEN);
  display.setTextSize(1);
  display.print("A1");
 }
  void a1Kirmizi()
 {
  
    display.setCursor(20,12);
  display.setTextColor(RED);
  display.setTextSize(1);
  display.print("A1");
 }
  void a2Yesil()
 {
  
    display.setCursor(68,12);
  display.setTextColor(GREEN);
  display.setTextSize(1);
  display.print("A2");
 }
  void a2Kirmizi()
 {
  
    display.setCursor(68,12);
  display.setTextColor(RED);
  display.setTextSize(1);
  display.print("A2");
 }
   void a3Yesil()
 {
  
    display.setCursor(20,44);
  display.setTextColor(GREEN);
  display.setTextSize(1);
  display.print("A3");
 }
  void a3Kirmizi()
 {
  
    display.setCursor(20,44);
  display.setTextColor(RED);
  display.setTextSize(1);
  display.print("A3");
 }
    void a4Yesil()
 {
  
    display.setCursor(68,44);
  display.setTextColor(GREEN);
  display.setTextSize(1);
  display.print("A4");
 }
  void a4Kirmizi()
 {
  
    display.setCursor(68,44);
  display.setTextColor(RED);
  display.setTextSize(1);
  display.print("A4");
 }
  
  
}
