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
#define SERVO_SIGNAL  5
#define RFID_RESET    6
#define OLED_RESET    7
#define OLED_DC       8
#define OLED_SS       9
#define RFID_SS       10
#define A_IN          A5

ShiftRegister74HC595<1> sr(SHIFT_DATA, SHIFT_CLOCK, SHIFT_LATCH);
Servo servo;
MFRC522 rfid(RFID_SS, RFID_RESET);
Adafruit_SSD1331 display = Adafruit_SSD1331(&SPI, OLED_SS, OLED_DC, OLED_RESET);

const int PRICE_PER_MINUTE = 10;

String CARDS[1][2] = {
  {"0", "00"}  
};

bool park_status[4] = {false, false, false, false};
bool park_status_cache[4] = {park_status};
unsigned long park_timer[4] = {0,0,0,0};

void setup() {

  servo.attach(SERVO_SIGNAL);
  Serial.begin(9600);
  SPI.begin();
  display.begin();    
  rfid.PCD_Init();
  
  display.fillScreen(BLACK);
}

void loop() {
  readLdrData();
  updateLEDs();
  updateTimers();
  displayEmptySpace();

  if (checkStatus() != -1) {
    readLdrData();
    updateLEDs();
    updateTimers();

    displayPrice(checkStatus());
    bool found = updateBalance(readCard(), getPrice(checkStatus()));
    if (!found) displayCNF();
    else openGate();
  }
}

int checkStatus() {
  for (int i = 0; i < 4; i++) {
    if (park_status_cache[i] != park_status[i]) return i;
  }
  return -1;
}

void readLdrData() {
  for (int i = 0; i < 4; i++) {

      sr.set(i, HIGH);
      
      int ldr_val = analogRead(A_IN);
      if (ldr_val > 200) park_status[i] = false;
      else park_status[i] = true;

      sr.set(i, LOW);
  }
}

void updateLEDs() {
    for (int i = 0; i < 4; i++) {
      
      if (park_status[i]) digitalWrite(i+4, HIGH);
      else digitalWrite(i+4, LOW);
        
    }
}

void updateTimers() {
  for (int i = 0; i < 4; i++) {
    if (park_status[i] && park_timer[i] == 0) park_timer[i] = millis();
    else if (!park_status && park_timer[i] != 0) park_timer[i] = 0;
  }
}

void resetTimers() {
  for (int i = 0; i < 4; i++) park_timer[i] = 0;
}

int getPrice(int park_num) {
  unsigned long elapsed_time = millis() - park_timer[park_num];
  unsigned long et_mins = round((elapsed_time / 1000) / 60);
  int price = PRICE_PER_MINUTE * et_mins;

  return price;
}

String readCard() {

  bool card_present = rfid.PICC_IsNewCardPresent();
  bool card_select = rfid.PICC_ReadCardSerial();

  while (!card_present && !card_select) {

    card_present = rfid.PICC_IsNewCardPresent();
    card_select = rfid.PICC_ReadCardSerial();
    
  }

  String UID = "";
  
  for (byte i = 0; i < rfid.uid.size; i++) 
  {
     UID.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
     UID.concat(String(rfid.uid.uidByte[i], HEX));
  }
  
  UID.toUpperCase();

  return UID;
}

bool updateBalance(String card_uid, int price) {
  bool found = false;
  for (int i = 0; i < sizeof(CARDS) / sizeof(int); i++) {
    if (CARDS[i][0] == card_uid) {
      found = true;
      CARDS[i][1] -= price;  
    }
  }

  return found;
}

void displayPrice(int park_num) {
  display.fillScreen(BLACK);
  display.setCursor(display.width() / 2, display.height() / 2);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print(String(getPrice(park_num)) + " TL");
}

void displayCNF() {
  display.fillScreen(BLACK);
  display.setCursor(display.width() / 2, display.height() / 2);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("CARD NOT FOUND!");
}

void displayEmptySpace() {
  display.fillScreen(BLACK);

  for (int i = 0; i < 4; i++) {
    if (park_status[i]) {
      if (i == 0) display.fillRect(5, 5, (display.width() / 2 - 5), (display.height() / 2 - 5), GREEN);
      else if (i == 1) display.fillRect((display.width() / 2 + 5), 5, display.width() - 5, (display.height() / 2 - 5), GREEN);
      else if (i == 0) display.fillRect(5, (display.height() / 2 + 5) , (display.width() / 2 - 5), display.height() - 5, GREEN);
      else if (i == 0) display.fillRect((display.width() / 2 + 5), (display.height() / 2 + 5) , display.width() - 5, display.height() - 5, GREEN);
    }
    else {
      if (i == 0) display.fillRect(5, 5, (display.width() / 2 - 5), (display.height() / 2 - 5), RED);
      else if (i == 1) display.fillRect((display.width() / 2 + 5), 5, display.width() - 5, (display.height() / 2 - 5), RED);
      else if (i == 0) display.fillRect(5, (display.height() / 2 + 5) , (display.width() / 2 - 5), display.height() - 5, RED);
      else if (i == 0) display.fillRect((display.width() / 2 + 5), (display.height() / 2 + 5) , display.width() - 5, display.height() - 5, RED);
    }
  }
}

void openGate() {
  servo.write(90);
  delay(5000);
  servo.write(0);
}
