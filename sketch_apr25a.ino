#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ShiftRegister74HC595.h>

#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

#define SHIFT_DATA    2
#define SHIFT_LATCH   3
#define SHIFT_CLOCK   4

#define SERVO_SIGNAL  1

#define RFID_RESET    6
#define RFID_SDA      5

#define OLED_RESET    8
#define OLED_DC       9
#define OLED_SS       10

#define A_IN          A5

void copy(bool* src, bool* dst, int len);

ShiftRegister74HC595<1> sr(SHIFT_DATA, SHIFT_CLOCK, SHIFT_LATCH);
Servo servo;
MFRC522 rfid(RFID_SDA, RFID_RESET);
Adafruit_SSD1331 display = Adafruit_SSD1331(&SPI, OLED_SS, OLED_DC, OLED_RESET);

const int PRICE_PER_MINUTE = 10;

String CARDS[1][2] = {
  {" 93 B7 1F 07", "3000"}  
};

bool park_status[4] = {false, false, false, false};
bool cache[4];
unsigned long park_timer[4] = {0,0,0,0};
int status;

void setup() {
  servo.attach(SERVO_SIGNAL);
  Serial.begin(9600);
  SPI.begin();
  display.begin();    
  rfid.PCD_Init();
  
  display.fillScreen(BLACK);
  sr.setAllLow();
  servo.write(0);

  readLdrData();
  updateLEDs();
  displayEmptySpace();
  update_cache();
}

void loop() {
  update_cache();
  readLdrData();
  
  if (isChanged()) {
    for (int i = 0; i < 4; i++) {
      if (park_status[i] != cache[i]) {
        if (park_status[i] && !cache[i]) {
          Serial.println("update");
          Serial.println(i);
          updateTimer(i);
          updateLEDs();
          displayEmptySpace();
        }
        else if (!park_status[i] && cache[i]) {
          auto duration = millis() - park_timer[i];
          int price = getPrice(duration); 
          
          bool found = false;
          while (!found) {
            displayPrice(price);

            String card = readCard();
            found = updateBalance(card, price);

            if (!found) {
              displayCNF();
              delay(2000);
            }
          }
          resetTimer(i);
          openGate();
          displayEmptySpace();
          updateLEDs();
        } 
      }
    }
  }
}

bool isChanged() {
  for (int i = 0; i < 4; i++) {
    if (park_status[i] != cache[i]) return true;
  }
  return false;
}

void readLdrData() {
  for (int i = 0; i < 4; i++) {

      sr.set(i, HIGH);
      
      int ldr_val = analogRead(A_IN);
      
      if (ldr_val > 100) park_status[i] = false;
      else park_status[i] = true;

      sr.set(i, LOW);
  }
}

void updateLEDs() {
    for (int i = 0; i < 4; i++) {
      if (!park_status[i]) sr.set(i+4, HIGH);
      else sr.set(i+4, LOW); 
    }
    Serial.println("LEDs Updated!");
}

void updateTimer(int timer) { 
  park_timer[timer] = millis();
}

void resetTimer(int timer) { 
  park_timer[timer] = 0;
}

int getPrice(int elapsed_time) {
  unsigned long et_mins = round((elapsed_time / 2000));
  int price = PRICE_PER_MINUTE * et_mins;

  return price;
}

String readCard() {

  while (true) {
    // Look for new cards
    if (!rfid.PICC_IsNewCardPresent()) 
    {
      continue;
    }
    // Select one of the cards
    if (!rfid.PICC_ReadCardSerial()) 
    {
      continue;
    }

    String content = "";
    
    for (byte i = 0; i < rfid.uid.size; i++) 
    {
       content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
       content.concat(String(rfid.uid.uidByte[i], HEX));
    }
    
    content.toUpperCase();
    return content;
  }
}

bool updateBalance(String card_uid, int price) {
  bool found = false;
  for (int i = 0; i < (sizeof(CARDS) / sizeof(*CARDS)); i++) {
    if (CARDS[i][0] == card_uid) {
      found = true;
      CARDS[i][1] = String(CARDS[i][1].toInt() - price);
      Serial.println("New Balance: " + String(CARDS[i][1]));
    }
  }

  return found;
}

void displayPrice(int price) {
  display.fillScreen(BLACK);
  display.setCursor(10, 10);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print(String(price) + " TL");
}

void displayCNF() {
  display.fillScreen(BLACK);
  display.setCursor(10, 10);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("CARD NOT FOUND!");
}

void displayEmptySpace() {
  display.fillScreen(BLACK);
  for (int i = 0; i < 4; i++) {
    if (!park_status[i]) {
      if (i == 0) display.fillRect(5, 5, (display.width() / 2 - 5), (display.height() / 2 - 5), GREEN);
      else if (i == 1) display.fillRect((display.width() / 2 + 5), 5, display.width() - 5, (display.height() / 2 - 5), GREEN);
      else if (i == 2) display.fillRect(5, (display.height() / 2 + 5) , (display.width() / 2 - 5), display.height() - 5, GREEN);
      else if (i == 3) display.fillRect((display.width() / 2 + 5), (display.height() / 2 + 5) , display.width() - 5, display.height() - 5, GREEN);
    }
    else {
      if (i == 0) display.fillRect(5, 5, (display.width() / 2 - 5), (display.height() / 2 - 5), RED);
      else if (i == 1) display.fillRect((display.width() / 2 + 5), 5, display.width() - 5, (display.height() / 2 - 5), RED);
      else if (i == 2) display.fillRect(5, (display.height() / 2 + 5) , (display.width() / 2 - 5), display.height() - 5, RED);
      else if (i == 3) display.fillRect((display.width() / 2 + 5), (display.height() / 2 + 5) , display.width() - 5, display.height() - 5, RED);
    }
  }
}

void openGate() {
  servo.write(90);
  delay(5000);
  servo.write(0);
}

void update_cache() {
  for (int i = 0; i < 4; i++) {
    cache[i] = park_status[i];  
  }
}
