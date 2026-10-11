// Otomatik Far Sensörü v3 - Yakup ULUTAŞ
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- PIN TANIMLARI ---
const uint8_t PIN_BTN_UP  = 8; // K1'in pini (D2 bozuk olduğu için D8'e alındı)
const uint8_t PIN_BTN_DWN = 3; // K2'nin pini
const uint8_t PIN_BTN_OK  = 4;
const uint8_t PIN_BTN_BCK = 9;

const uint8_t PIN_PARK    = 5;
const uint8_t PIN_KISA    = 6;
const uint8_t PIN_PWM     = 11; // TIP121 / 12V LED Sürücü

// --- LDR PİN SEÇENEKLERİ ---
// Uno/Nano'da A4/A5 OLED (I2C) için ayrılmıştır, o yüzden A0,A1,A2,A3,A6,A7 seçilebilir.
const uint8_t ANALOG_PINS[] = {A0, A1, A2, A3, A6, A7};
const uint8_t ANALOG_COUNT  = 6;

// --- SİMGELER (24x20 Bitmap) ---
const unsigned char PROGMEM icon_park_on[] = {
  0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x04, 0x00, 0x10, 0x02, 0x00, 0x20, 0x00, 0x08, 0x00, 0x00, 0x7f, 0x00, 0x00, 0xff, 0x80, 0x00, 0xff, 0x80, 0x00, 0xff, 0x80, 0x39, 0xff, 0xce, 0x00, 0xff, 0x80, 0x00, 0xff, 0x80, 0x00, 0xff, 0x80, 0x00, 0x7f, 0x00, 0x00, 0x08, 0x00, 0x02, 0x00, 0x20, 0x04, 0x00, 0x10, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00
};
const unsigned char PROGMEM icon_park_off[] = {
  0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x04, 0x00, 0x10, 0x02, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x55, 0x00, 0x00, 0xaa, 0x80, 0x00, 0x55, 0x00, 0x28, 0xaa, 0x8a, 0x00, 0x55, 0x00, 0x00, 0xaa, 0x80, 0x00, 0x55, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x20, 0x04, 0x00, 0x10, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00
};
const unsigned char PROGMEM icon_kisa_on[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x9f, 0xc0, 0x07, 0x9f, 0xe0, 0x0f, 0x1f, 0xf8, 0x78, 0x1f, 0xf8, 0xf0, 0x1f, 0xfc, 0x81, 0x9f, 0xfc, 0x03, 0x9f, 0xfc, 0x1e, 0x1f, 0xfc, 0x3c, 0x1f, 0xfc, 0xe0, 0x1f, 0xfc, 0xc0, 0x9f, 0xf8, 0x07, 0x9f, 0xf8, 0x0f, 0x1f, 0xe0, 0x78, 0x1f, 0xc0, 0xf0, 0x1e, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned char PROGMEM icon_kisa_off[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x15, 0x40, 0x02, 0x8a, 0xa0, 0x05, 0x15, 0x50, 0x28, 0x0a, 0xa8, 0x50, 0x15, 0x54, 0x80, 0x8a, 0xa8, 0x01, 0x15, 0x54, 0x0a, 0x0a, 0xa8, 0x14, 0x15, 0x54, 0xa0, 0x0a, 0xa8, 0x40, 0x15, 0x50, 0x02, 0x8a, 0xa8, 0x05, 0x15, 0x40, 0x28, 0x0a, 0x80, 0x50, 0x14, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned char PROGMEM icon_drl_on[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x1f, 0xe0, 0x00, 0x1f, 0xf8, 0x00, 0x1f, 0xf8, 0x00, 0x1f, 0xfc, 0x00, 0x1f, 0xfc, 0x00, 0x1f, 0xfc, 0x00, 0x1f, 0xfc, 0x00, 0x1f, 0xfc, 0x00, 0x1f, 0xfc, 0x00, 0x1f, 0xf8, 0x00, 0x1f, 0xf8, 0x00, 0x1f, 0xe0, 0x00, 0x1f, 0xc0, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned char PROGMEM icon_drl_off[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x15, 0x40, 0x00, 0x0a, 0xa0, 0x00, 0x15, 0x50, 0x00, 0x0a, 0xa8, 0x00, 0x15, 0x54, 0x00, 0x0a, 0xa8, 0x00, 0x15, 0x54, 0x00, 0x0a, 0xa8, 0x00, 0x15, 0x54, 0x00, 0x0a, 0xa8, 0x00, 0x15, 0x50, 0x00, 0x0a, 0xa8, 0x00, 0x15, 0x40, 0x00, 0x0a, 0x80, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// --- MENÜ ETİKETLERİ (Flash'ta) ---
const char L0[]  PROGMEM = "Sistem Durumu";
const char L1[]  PROGMEM = "Sensor Pin";
const char L2[]  PROGMEM = "Park Esik";
const char L3[]  PROGMEM = "Kisa Esik";
const char L4[]  PROGMEM = "Histerezis";
const char L5[]  PROGMEM = "Park Ac Gec";
const char L6[]  PROGMEM = "Park Kap Gec";
const char L7[]  PROGMEM = "Kisa Ac Gec";
const char L8[]  PROGMEM = "Kisa Kap Gec";
const char L9[]  PROGMEM = "DRL Gunduz PWM";
const char L10[] PROGMEM = "DRL Tam Esik";
const char L11[] PROGMEM = "DRL Kisa PWM";
const char L12[] PROGMEM = "Ekran Koruyucu";
const char L13[] PROGMEM = "Sensor Hizi";
const char L14[] PROGMEM = "Mars Gecikme";
const char L15[] PROGMEM = "Fabrika Ayari";
const char L16[] PROGMEM = "Hakkinda";
const char L17[] PROGMEM = "Test Modu";
const char L18[] PROGMEM = "Menuden Cik";
const char* const menuLabels[] PROGMEM = {L0,L1,L2,L3,L4,L5,L6,L7,L8,L9,L10,L11,L12,L13,L14,L15,L16,L17,L18};

// --- EEPROM AYARLARI ---
struct Settings {
  uint16_t magic;
  uint8_t  version;
  int16_t  parkThreshold;
  int16_t  kisaThreshold;
  int16_t  hysteresis;
  uint16_t parkOnDelay;
  uint16_t parkOffDelay;
  uint16_t kisaOnDelay;
  uint16_t kisaOffDelay;
  uint8_t  drlDayPwm;       // DRL en parlak değer
  int16_t  drlFullLdr;      // DRL'nin tam parlaklığa ulaştığı LDR değeri
  uint8_t  drlKisaPwm;      // DRL en kısık değer
  uint8_t  sensorPin;       // LDR pini (ANALOG_PINS dizisindeki indeks)
  uint8_t  screenSaver;     // 0:Yok, 1:1dk, 2:5dk, 3:30dk
  uint8_t  sensorSpeed;     // 0:Cok Yavas, 1:Yavas, 2:Orta, 3:Hizli, 4:Cok Hizli
  uint8_t  bootDelaySec;    // 1-15 saniye mars gecikmesi
  uint16_t crc;
};

Settings config;
const uint16_t EEPROM_MAGIC = 0xA55B;
const uint8_t  CONFIG_VERSION = 11; // v3: yapı değişti (ayarlar bir kez varsayılana döner)

// Pin dizisi indeksi -> ekranda görünen analog pin numarası (A0..A3, A6, A7)
// NOT: Arduino'nun otomatik prototip eklemesi dosyadaki İLK fonksiyondan önce çalışır.
// Bu fonksiyon struct Settings'ten SONRA durmalı.
uint8_t pinNum(uint8_t idx) { return idx < 4 ? idx : idx + 2; }

uint16_t calculateCRC(const Settings &data) {
  const uint8_t *ptr = (const uint8_t *)&data;
  size_t len = sizeof(Settings) - sizeof(uint16_t);
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < len; i++) {
    crc ^= (uint16_t)ptr[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x0001) crc = (crc >> 1) ^ 0xA001;
      else crc = (crc >> 1);
    }
  }
  return crc;
}

void loadDefaults() {
  config.magic = EEPROM_MAGIC;
  config.version = CONFIG_VERSION;
  config.parkThreshold = 400;
  config.kisaThreshold = 170;
  config.hysteresis = 40;
  config.parkOnDelay = 2000;
  config.parkOffDelay = 6000;
  config.kisaOnDelay = 1500;
  config.kisaOffDelay = 4000;
  config.drlDayPwm = 100;
  config.drlFullLdr = 700;
  config.drlKisaPwm = 58;
  config.sensorPin = 0;             // A0
  config.screenSaver = 1;           // Varsayılan: 1 Dakika
  config.sensorSpeed = 2;           // Varsayilan: Orta
  config.bootDelaySec = 5;          // Varsayilan: 5 Saniye
  config.crc = calculateCRC(config);
}

void saveSettings() {
  config.crc = calculateCRC(config);
  EEPROM.put(0, config);
}

void readSettings() {
  EEPROM.get(0, config);
  if (config.magic != EEPROM_MAGIC || config.version != CONFIG_VERSION || config.crc != calculateCRC(config)) {
    loadDefaults();
    saveSettings();
  }
}

// --- DURUM DEĞİŞKENLERİ ---
bool systemEnabled = true;      // Geçici kapatma: EEPROM'a yazılmaz, her açılışta AKTİF başlar

int filteredLdr = 500;          // Süzülmüş LDR değeri (0-1023)
int ldrAcc = 500 << 4;          // Filtre durumu (x16 hassasiyet)

bool parkActive = false;
bool kisaActive = false;
uint8_t currentPwm = 0;
uint8_t rawPwm = 0;

unsigned long tParkDarkStart = 0;
unsigned long tParkLightStart = 0;
unsigned long tKisaDarkStart = 0;
unsigned long tKisaLightStart = 0;

unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;

unsigned long lastButtonPress = 0;
bool displayOff = false;        // Ekran koruyucu aktif (animasyon veya tam karartma)
bool saverBlank = false;        // Koruyucunun 2. aşaması: ekran tamamen kapalı
const unsigned long SAVER_ANIM_MS = 600000UL; // Animasyon 10 dk sürer, sonra ekran tamamen kapanır
const uint8_t SAVER_CONTRAST = 0x30;          // Animasyon parlaklığı (0x00-0xFF). Çok sönük görünürse artır.

// --- MENÜ MİMARİSİ ---
enum MenuState { STATE_MAIN, STATE_MENU, STATE_EDIT, STATE_CONFIRM, STATE_INFO, STATE_TEST };

MenuState menuState = STATE_MAIN;
int8_t currentMenuIndex = 0;
const int8_t TOTAL_MENU_ITEMS = 19;
const int8_t IDX_FACTORY = 15;
const int8_t IDX_INFO = 16;
const int8_t IDX_TEST = 17;
const int8_t IDX_EXIT = 18;

bool btnUpPressed = false;
bool btnDwnPressed = false;
bool btnOkPressed = false;
bool btnBckPressed = false;

void readButtons() {
  btnUpPressed  = false;
  btnDwnPressed = false;
  btnOkPressed  = false;
  btnBckPressed = false;

  static unsigned long lastDebounce = 0;
  if (millis() - lastDebounce < 40) return;
  lastDebounce = millis();

  static bool pUp = HIGH, pDwn = HIGH, pOk = HIGH, pBck = HIGH;

  bool cUp  = digitalRead(PIN_BTN_UP);
  bool cDwn = digitalRead(PIN_BTN_DWN);
  bool cOk  = digitalRead(PIN_BTN_OK);
  bool cBck = digitalRead(PIN_BTN_BCK);

  btnUpPressed  = (pUp == HIGH && cUp == LOW);
  btnDwnPressed = (pDwn == HIGH && cDwn == LOW);
  btnOkPressed  = (pOk == HIGH && cOk == LOW);
  btnBckPressed = (pBck == HIGH && cBck == LOW);

  if (btnUpPressed || btnDwnPressed || btnOkPressed || btnBckPressed) {
    lastButtonPress = millis();
    if (displayOff) {
      displayOff = false;
      saverBlank = false;
      display.dim(false);                 // parlaklığı geri getir
      btnUpPressed = btnDwnPressed = btnOkPressed = btnBckPressed = false; // Sadece uyandır
    }
  }

  pUp = cUp; pDwn = cDwn; pOk = cOk; pBck = cBck;
}

// --- LDR SENSÖRÜ ---
// Bloklamayan okuma: 50 ms'de bir 4 örneklik "salvo", örnekler arası 3 ms (delay YOK).
// Salvo sürerken ekran güncellenmez, böylece örnek aralıkları düzenli kalır.
const uint8_t  SPEED_DIV[5]       = {16, 8, 4, 2, 1}; // Çok Yavaş ... Çok Hızlı
const uint8_t  SAMPLES_PER_BURST  = 4;
const unsigned long BURST_PERIOD_MS = 50;
const unsigned long SAMPLE_GAP_MS   = 3;

bool sensorBusy = false;
uint8_t sampleN = 0;
int sampleSum = 0;
unsigned long lastSampleMs = 0;

void initSensors() {
  analogRead(ANALOG_PINS[config.sensorPin]);   // ADC MUX geçişi için boş okuma
  filteredLdr = analogRead(ANALOG_PINS[config.sensorPin]);
  ldrAcc = filteredLdr << 4;
  sensorBusy = false;
}

void updateSensor(unsigned long now) {
  if (!sensorBusy) {
    if (now - lastSensorRead < BURST_PERIOD_MS) return;
    lastSensorRead = now;
    sensorBusy = true;
    sampleN = 0;
    sampleSum = 0;
    lastSampleMs = now - SAMPLE_GAP_MS;      // ilk örnek hemen alınsın
  }

  if (now - lastSampleMs < SAMPLE_GAP_MS) return;
  lastSampleMs = now;

  analogRead(ANALOG_PINS[config.sensorPin]); // boş okuma (MUX oturması)
  sampleSum += analogRead(ANALOG_PINS[config.sensorPin]);
  if (++sampleN < SAMPLES_PER_BURST) return;

  // Salvo bitti: ortalama al, hız ayarına göre süz
  sensorBusy = false;
  uint8_t div = SPEED_DIV[config.sensorSpeed < 5 ? config.sensorSpeed : 2];
  int avg = sampleSum / SAMPLES_PER_BURST;
  ldrAcc += ((avg << 4) - ldrAcc) / div;     // x16 hassasiyet: küçük farklarda takılma olmaz
  filteredLdr = (ldrAcc + 8) >> 4;
}

// --- DRL: ORTAM IŞIĞINA GÖRE YUMUŞAK PARLAKLIK ---
unsigned long lastPwmStep = 0;
const unsigned long DRL_STEP_MS = 7; // Her adim 7 ms -> 0'dan 255'e tam donanim cozunurlugunde ~1.8 saniyede zarif fade-in

uint8_t computeDrlTarget() {
  if (kisaActive) return config.drlKisaPwm;
  int lo = config.kisaThreshold;
  int hi = config.drlFullLdr;
  if (hi < lo + 10) hi = lo + 10;
  long v = constrain(filteredLdr, lo, hi);
  return (uint8_t)map(v, lo, hi, config.drlKisaPwm, config.drlDayPwm);
}

void updateDrlPwm(unsigned long now) {
  if (now - lastPwmStep < DRL_STEP_MS) return;
  lastPwmStep = now;

  uint8_t targetPercent = computeDrlTarget();
  uint8_t targetRaw = map(targetPercent, 0, 100, 0, 255);

  if (rawPwm < targetRaw) rawPwm++;
  else if (rawPwm > targetRaw) rawPwm--;

  analogWrite(PIN_PWM, rawPwm);
  currentPwm = (uint8_t)(((uint16_t)rawPwm * 100 + 127) / 255);
}

// --- FAR MANTIĞI ---
void processLights(unsigned long now) {
  // Sistem geçici olarak kapatıldıysa her şey kapalı
  if (!systemEnabled) {
    parkActive = false;
    kisaActive = false;
    tParkDarkStart = tParkLightStart = tKisaDarkStart = tKisaLightStart = 0;
    currentPwm = 0;
    rawPwm = 0;
    digitalWrite(PIN_PARK, LOW);
    digitalWrite(PIN_KISA, LOW);
    analogWrite(PIN_PWM, 0);
    return;
  }

  if (now < (unsigned long)config.bootDelaySec * 1000UL) {
    // Mars Gecikmesi suresince farlari kapali tut
    currentPwm = 0;
    rawPwm = 0;
    digitalWrite(PIN_PARK, LOW);
    digitalWrite(PIN_KISA, LOW);
    analogWrite(PIN_PWM, 0);
    return;
  }

  // 1. Park Kontrolü
  int parkOnLevel = config.parkThreshold;
  int parkOffLevel = config.parkThreshold + config.hysteresis;

  if (filteredLdr < parkOnLevel) {
    tParkLightStart = 0;
    if (!parkActive) {
      if (tParkDarkStart == 0) tParkDarkStart = now;
      if (now - tParkDarkStart >= config.parkOnDelay) parkActive = true;
    }
  } else if (filteredLdr > parkOffLevel) {
    tParkDarkStart = 0;
    if (parkActive) {
      if (tParkLightStart == 0) tParkLightStart = now;
      if (now - tParkLightStart >= config.parkOffDelay) parkActive = false;
    }
  } else {
    tParkDarkStart = 0;
    tParkLightStart = 0;
  }

  // 2. Kısa Far Kontrolü
  int kisaOnLevel = config.kisaThreshold;
  int kisaOffLevel = config.kisaThreshold + config.hysteresis;

  // --- TÜNEL MODU (Düşüş Hızı Algılayıcısı) ---
  static unsigned long tunnelCheckTimer = 0;
  static int oldLdrForTunnel = 500;
  if (now - tunnelCheckTimer >= 500) {
    int drop = oldLdrForTunnel - filteredLdr;
    if (drop >= 150 && filteredLdr < kisaOnLevel) {
       // Aniden karanlık oldu (Tünel), sayaçları ileri sar!
       if (tParkDarkStart > 0) tParkDarkStart = now - config.parkOnDelay;
       if (tKisaDarkStart > 0) tKisaDarkStart = now - config.kisaOnDelay;
    }
    oldLdrForTunnel = filteredLdr;
    tunnelCheckTimer = now;
  }
  // ---------------------------------------------

  if (filteredLdr < kisaOnLevel) {
    tKisaLightStart = 0;
    if (!kisaActive) {
      if (tKisaDarkStart == 0) tKisaDarkStart = now;
      if (now - tKisaDarkStart >= config.kisaOnDelay) kisaActive = true;
    }
  } else if (filteredLdr > kisaOffLevel) {
    tKisaDarkStart = 0;
    if (kisaActive) {
      if (tKisaLightStart == 0) tKisaLightStart = now;
      if (now - tKisaLightStart >= config.kisaOffDelay) kisaActive = false;
    }
  } else {
    tKisaDarkStart = 0;
    tKisaLightStart = 0;
  }

  digitalWrite(PIN_PARK, parkActive ? HIGH : LOW);
  digitalWrite(PIN_KISA, kisaActive ? HIGH : LOW);

  updateDrlPwm(now);
}

// --- MENÜ DEĞERİNİ METNE ÇEVİR ---
void fmtSeconds(char *buf, size_t n, uint16_t ms) {
  snprintf_P(buf, n, PSTR("%u.%02us"), (unsigned)(ms / 1000), (unsigned)((ms % 1000) / 10));
}

void getSettingText(int8_t idx, char *buf, size_t n) {
  switch (idx) {
    case 0:  strcpy_P(buf, systemEnabled ? PSTR("AKTIF") : PSTR("KAPALI")); break;
    case 1:  snprintf_P(buf, n, PSTR("A%u"), (unsigned)pinNum(config.sensorPin)); break;
    case 2:  snprintf_P(buf, n, PSTR("%d"), (int)config.parkThreshold); break;
    case 3:  snprintf_P(buf, n, PSTR("%d"), (int)config.kisaThreshold); break;
    case 4:  snprintf_P(buf, n, PSTR("%d"), (int)config.hysteresis); break;
    case 5:  fmtSeconds(buf, n, config.parkOnDelay); break;
    case 6:  fmtSeconds(buf, n, config.parkOffDelay); break;
    case 7:  fmtSeconds(buf, n, config.kisaOnDelay); break;
    case 8:  fmtSeconds(buf, n, config.kisaOffDelay); break;
    case 9:  snprintf_P(buf, n, PSTR("%%%u"), (unsigned)config.drlDayPwm); break;
    case 10: snprintf_P(buf, n, PSTR("%d"), (int)config.drlFullLdr); break;
    case 11: snprintf_P(buf, n, PSTR("%%%u"), (unsigned)config.drlKisaPwm); break;
    case 12:
      if (config.screenSaver == 0) strcpy_P(buf, PSTR("Yok"));
      else if (config.screenSaver == 1) strcpy_P(buf, PSTR("1 dk"));
      else if (config.screenSaver == 2) strcpy_P(buf, PSTR("5 dk"));
      else strcpy_P(buf, PSTR("30 dk"));
      break;
    case 13:
      if (config.sensorSpeed == 0) strcpy_P(buf, PSTR("C.Yavas"));
      else if (config.sensorSpeed == 1) strcpy_P(buf, PSTR("Yavas"));
      else if (config.sensorSpeed == 2) strcpy_P(buf, PSTR("Orta"));
      else if (config.sensorSpeed == 3) strcpy_P(buf, PSTR("Hizli"));
      else strcpy_P(buf, PSTR("C.Hizli"));
      break;
    case 14: snprintf_P(buf, n, PSTR("%u sn"), (unsigned)config.bootDelaySec); break;
    default: strcpy_P(buf, PSTR("")); break;
  }
}

// Düzenleme ekranındaki kaydırıcı için (değer, min, max). false: kaydırıcı yok.
bool getSettingRange(int8_t idx, long &val, long &lo, long &hi) {
  switch (idx) {
    case 2:  val = config.parkThreshold; lo = 10; hi = 1000;  return true;
    case 3:  val = config.kisaThreshold; lo = 10; hi = 1000;  return true;
    case 4:  val = config.hysteresis;    lo = 0;  hi = 200;   return true;
    case 5:  val = config.parkOnDelay;   lo = 0;  hi = 30000; return true;
    case 6:  val = config.parkOffDelay;  lo = 0;  hi = 30000; return true;
    case 7:  val = config.kisaOnDelay;   lo = 0;  hi = 30000; return true;
    case 8:  val = config.kisaOffDelay;  lo = 0;  hi = 30000; return true;
    case 9:  val = config.drlDayPwm;     lo = 0;  hi = 100;   return true;
    case 10: val = config.drlFullLdr;    lo = 10; hi = 1000;  return true;
    case 11: val = config.drlKisaPwm;    lo = 0;  hi = 100;   return true;
    case 12: val = config.screenSaver;   lo = 0;  hi = 3;     return true;
    case 13: val = config.sensorSpeed;   lo = 0;  hi = 4;     return true;
    case 14: val = config.bootDelaySec;  lo = 1;  hi = 15;    return true;
    default: return false;
  }
}

void cycleSensorPin(int dir) {
  config.sensorPin = (config.sensorPin + ANALOG_COUNT + dir) % ANALOG_COUNT;
}

// ================= EKRAN YARDIMCILARI =================
void printLabel(int8_t i) {
  display.print((const __FlashStringHelper *)pgm_read_ptr(&menuLabels[i]));
}

// Üst başlık şeridi (ters renk). Başlığı yazdıktan sonra endHeader() çağır.
void beginHeader() {
  display.fillRect(0, 0, 128, 9, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(3, 1);
}
void endHeader() { display.setTextColor(SSD1306_WHITE); }

void beginFooter() {
  display.drawFastHLine(0, 56, 128, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 57);
}

// LDR ışık çubuğu (çerçeveli, içi dolu)
void drawLdrBar(uint8_t y, uint8_t h, int value) {
  display.drawRect(0, y, 128, h, SSD1306_WHITE);
  int w = map(constrain(value, 0, 1023), 0, 1023, 0, 124);
  display.fillRect(2, y + 2, w, h - 4, SSD1306_WHITE);
}

// ================= ANA EKRAN =================
void drawDrlIcon(int x, int y, uint8_t pwm, uint16_t color) {
  if (pwm == 0) {
    display.drawBitmap(x, y, icon_drl_off, 24, 20, color);
    return;
  }
  display.drawBitmap(x, y, icon_drl_on, 24, 20, color);
  uint8_t len = 1 + ((uint16_t)pwm * 8) / 100;
  const uint8_t rayY[3] = {3, 8, 13};
  for (uint8_t i = 0; i < 3; i++) {
    display.fillRect(x + 9 - len, y + rayY[i], len, 2, color);
  }
}

// type: 0=Park, 1=Kisa, 2=DRL | prog: -1 = cubuk yok, 0..100 = cubuk dolulugu
void drawLightCard(int x, uint8_t type, const __FlashStringHelper *label,
                   bool on, uint8_t pwm, const char *status, int8_t prog) {
  const uint8_t CARD_Y = 11, CARD_W = 40, CARD_H = 44;
  uint16_t fg = on ? SSD1306_BLACK : SSD1306_WHITE;

  if (on) display.fillRoundRect(x, CARD_Y, CARD_W, CARD_H, 4, SSD1306_WHITE);
  else    display.drawRoundRect(x, CARD_Y, CARD_W, CARD_H, 4, SSD1306_WHITE);

  int ix = x + 8, iy = CARD_Y + 2;
  if (type == 0)      display.drawBitmap(ix, iy, on ? icon_park_on : icon_park_off, 24, 20, fg);
  else if (type == 1) display.drawBitmap(ix, iy, on ? icon_kisa_on : icon_kisa_off, 24, 20, fg);
  else                drawDrlIcon(ix, iy, pwm, fg);

  display.setTextSize(1);
  display.setTextColor(fg);

  uint8_t ll = strlen_P((const char *)label);
  display.setCursor(x + (CARD_W - ll * 6) / 2, CARD_Y + 23);
  display.print(label);

  uint8_t sl = strlen(status);
  display.setCursor(x + (CARD_W - sl * 6) / 2, CARD_Y + 31);
  display.print(status);

  if (prog >= 0) {
    display.drawFastHLine(x + 4, CARD_Y + 40, CARD_W - 8, fg);                 // ince ray
    display.fillRect(x + 4, CARD_Y + 39, ((CARD_W - 8) * prog) / 100, 3, fg);  // dolu kisim
  }
  display.setTextColor(SSD1306_WHITE);
}

// Gecikme sayacini ve durum metnini hazirlar
void lightStatus(char *st, size_t n, bool active,
                 unsigned long darkStart, unsigned long lightStart,
                 unsigned long onDelay, unsigned long offDelay,
                 unsigned long now, int8_t &prog) {
  prog = -1;
  unsigned long start = 0, total = 0;
  if (!active && darkStart > 0)      { start = darkStart;  total = onDelay;  }
  else if (active && lightStart > 0) { start = lightStart; total = offDelay; }

  if (start > 0 && total > 0 && (now - start) < total) {
    unsigned long el = now - start;
    prog = (int8_t)((el * 100UL) / total);
    long rem = (long)((total - el + 999) / 1000);
    if (active) snprintf_P(st, n, PSTR("KAP %ld"), rem);
    else        snprintf_P(st, n, PSTR("AC %ld"), rem);
    return;
  }
  strcpy_P(st, active ? PSTR("ACIK") : PSTR("KAPALI"));
}

void drawMainScreen() {
  unsigned long now = millis();
  bool blink = (now / 500) % 2;

  // ---- ÜST ŞERİT: durum rozeti + sensör değeri ----
  display.setTextSize(1);
  uint8_t bw = systemEnabled ? 28 : 40;
  if (systemEnabled || blink) {
    display.fillRect(0, 0, bw, 9, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  } else {
    display.drawRect(0, 0, bw, 9, SSD1306_WHITE);
    display.setTextColor(SSD1306_WHITE);
  }
  display.setCursor(2, 1);
  display.print(systemEnabled ? F("AUTO") : F("KAPALI"));
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(bw + 6, 1);
  display.print('A');
  display.print(pinNum(config.sensorPin));
  display.print(':');
  display.print(filteredLdr);

  if (systemEnabled) {
    // ---- ÜÇ KART: PARK | KISA | DRL ----
    char st[8];
    int8_t prog;

    lightStatus(st, sizeof(st), parkActive, tParkDarkStart, tParkLightStart,
                config.parkOnDelay, config.parkOffDelay, now, prog);
    drawLightCard(0, 0, F("PARK"), parkActive, 0, st, prog);

    lightStatus(st, sizeof(st), kisaActive, tKisaDarkStart, tKisaLightStart,
                config.kisaOnDelay, config.kisaOffDelay, now, prog);
    drawLightCard(44, 1, F("KISA"), kisaActive, 0, st, prog);

    snprintf_P(st, sizeof(st), PSTR("%%%u"), (unsigned)currentPwm);
    drawLightCard(88, 2, F("DRL"), currentPwm > 0, currentPwm, st, (int8_t)currentPwm);

    // ---- EŞİK İŞARETLERİ (Kısa ve Park) ----
    int kx = 2 + (int)((long)constrain(config.kisaThreshold, 0, 1023) * 124 / 1023);
    int px = 2 + (int)((long)constrain(config.parkThreshold, 0, 1023) * 124 / 1023);
    display.drawFastVLine(kx, 56, 2, SSD1306_WHITE);
    display.drawFastVLine(px, 56, 2, SSD1306_WHITE);
  } else {
    // ---- SİSTEM KAPALI UYARISI ----
    display.setTextSize(2);
    display.setCursor(28, 14);
    display.print(F("SISTEM"));
    if (blink) {
      display.fillRect(26, 30, 76, 16, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    }
    display.setCursor(28, 32);
    display.print(F("KAPALI"));
    display.setTextColor(SSD1306_WHITE);
  }

  // ---- ALT: LDR IŞIK GÖSTERGESİ ----
  drawLdrBar(58, 6, filteredLdr);
}

// ================= MENÜ EKRANLARI =================
void drawMenu() {
  beginHeader();
  display.print(F("AYARLAR"));
  char c[8];
  snprintf_P(c, sizeof(c), PSTR("%d/%d"), (int)currentMenuIndex + 1, (int)TOTAL_MENU_ITEMS);
  display.setCursor(125 - strlen(c) * 6, 1);
  display.print(c);
  endHeader();

  int8_t start = constrain(currentMenuIndex - 1, 0, TOTAL_MENU_ITEMS - 4);
  for (int8_t i = 0; i < 4; i++) {
    int8_t idx = start + i;
    int y = 12 + i * 11;
    bool sel = (idx == currentMenuIndex);

    if (sel) {
      display.fillRoundRect(0, y, 123, 10, 2, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(4, y + 1);
    printLabel(idx);

    char buf[12];
    if (idx >= IDX_FACTORY) strcpy_P(buf, PSTR(">"));
    else getSettingText(idx, buf, sizeof(buf));
    display.setCursor(119 - strlen(buf) * 6, y + 1);
    display.print(buf);
  }
  display.setTextColor(SSD1306_WHITE);

  // Kaydırma çubuğu
  for (int y = 12; y < 56; y += 2) display.drawPixel(127, y, SSD1306_WHITE);
  const int th = 10;
  int ty = 12 + ((44 - th) * currentMenuIndex) / (TOTAL_MENU_ITEMS - 1);
  display.fillRect(126, ty, 2, th, SSD1306_WHITE);

  beginFooter();
  display.write(0x1E); display.write(0x1F);
  display.print(F(" Gez OK Sec "));
  display.write(0x11);
  display.print(F(" Geri"));
}

void drawEdit() {
  beginHeader();
  printLabel(currentMenuIndex);
  endHeader();

  char buf[12];
  getSettingText(currentMenuIndex, buf, sizeof(buf));
  display.setTextSize(2);
  display.setCursor((128 - strlen(buf) * 12) / 2, 21);
  display.print(buf);
  display.setTextSize(1);
  display.fillTriangle(4, 28, 11, 22, 11, 34, SSD1306_WHITE);
  display.fillTriangle(123, 28, 116, 22, 116, 34, SSD1306_WHITE);

  long val, lo, hi;
  if (getSettingRange(currentMenuIndex, val, lo, hi) && hi > lo) {
    display.drawRect(8, 43, 112, 7, SSD1306_WHITE);
    int w = (int)(((val - lo) * 108L) / (hi - lo));
    display.fillRect(10, 45, w, 3, SSD1306_WHITE);
  }

  beginFooter();
  display.write(0x1E); display.write(0x1F);
  display.print(F(" Degis  OK Kaydet"));
}

void drawConfirm() {
  beginHeader();
  display.print(F("FABRIKA AYARI"));
  endHeader();

  display.setCursor(31, 20);
  display.print(F("Tum ayarlar"));
  display.setCursor(19, 32);
  display.print(F("sifirlansin mi?"));

  beginFooter();
  display.print(F("OK Evet     "));
  display.write(0x11);
  display.print(F(" Hayir"));
}

void drawInfo() {
  beginHeader();
  display.print(F("HAKKINDA"));
  endHeader();

  display.setTextSize(2);
  display.setCursor(4, 16);
  display.print(F("OTO FAR v3"));
  display.setTextSize(1);
  display.setCursor(28, 38);
  display.print(F("Yakup ULUTAS"));
  display.setCursor(19, 48);
  display.print(F("yakupulutas.com"));

  beginFooter();
  display.write(0x11);
  display.print(F(" Geri"));
}

void drawTest() {
  beginHeader();
  display.print(F("LDR TEST MODU"));
  endHeader();

  int d = filteredLdr >= 1000 ? 4 : (filteredLdr >= 100 ? 3 : (filteredLdr >= 10 ? 2 : 1));
  display.setTextSize(3);
  display.setCursor((128 - d * 18) / 2, 15);
  display.print(filteredLdr);
  drawLdrBar(42, 8, filteredLdr);

  beginFooter();
  display.write(0x11);
  display.print(F(" Cikis"));
}

void drawOled() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  switch (menuState) {
    case STATE_MAIN:    drawMainScreen(); break;
    case STATE_MENU:    drawMenu();       break;
    case STATE_EDIT:    drawEdit();       break;
    case STATE_CONFIRM: drawConfirm();    break;
    case STATE_INFO:    drawInfo();       break;
    case STATE_TEST:    drawTest();       break;
  }

  display.display();
}

// ================= AÇILIŞ ANİMASYONU =================
const uint8_t SIN_T[] PROGMEM = {0, 17, 34, 50, 64, 77, 87, 94, 98, 100};
const char SPLASH_SIG[] PROGMEM = "by Yakup ULUTAS";
const unsigned long SPLASH_MS = 3600;

int8_t sinK(uint8_t k) {            // sin(k * 10 derece) * 100
  k %= 36;
  int8_t sgn = 1;
  if (k > 18) { k -= 18; sgn = -1; }
  if (k > 9) k = 18 - k;
  return sgn * (int8_t)pgm_read_byte(&SIN_T[k]);
}
int8_t cosK(uint8_t k) { return sinK(k + 9); }

void drawSplashFrame(unsigned long t) {
  const int cx = 26, cy = 27;
  display.clearDisplay();
  display.setTextWrap(false);
  display.setTextColor(SSD1306_WHITE);

  // 1) Tik halkası + tarama ibresi (0-1100 ms)
  uint8_t lit = (t >= 1100) ? 36 : (uint8_t)(t * 36UL / 1100UL);
  for (uint8_t k = 0; k < 36; k++) {
    int xo = cx + (23 * sinK(k)) / 100;
    int yo = cy - (23 * cosK(k)) / 100;
    if (k < lit) {
      int ri = (k % 9 == 0) ? 16 : 19;
      display.drawLine(cx + (ri * sinK(k)) / 100, cy - (ri * cosK(k)) / 100, xo, yo, SSD1306_WHITE);
    } else {
      display.drawPixel(xo, yo, SSD1306_WHITE);
    }
  }
  if (lit < 36) {   // öndeki uzun tik = ibre
    display.drawLine(cx + (13 * sinK(lit)) / 100, cy - (13 * cosK(lit)) / 100,
                     cx + (23 * sinK(lit)) / 100, cy - (23 * cosK(lit)) / 100, SSD1306_WHITE);
  }

  // 2) Halka bitince far yanar (yanıp sönerek)
  bool ledOn = (t >= 1100) && !((t < 1400) && (((t - 1100) / 70) % 2));
  display.drawBitmap(cx - 12, cy - 10, ledOn ? icon_kisa_on : icon_kisa_off, 24, 20, SSD1306_WHITE);

  // 3) Başlık, tarama çizgisiyle ortaya çıkar (1100-1900 ms)
  if (t >= 1100) {
    int scanX = 56 + (int)((t - 1100) * 72UL / 800UL);
    if (scanX > 128) scanX = 128;

    display.setTextSize(3);
    display.setCursor(59, 3);
    display.print(F("OTO"));
    display.fillRect(56, 26, 60, 24, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(59, 28);
    display.print(F("FAR"));
    display.setTextColor(SSD1306_WHITE);

    if (scanX < 128) {
      display.fillRect(scanX, 2, 128 - scanX, 49, SSD1306_BLACK);
      display.drawFastVLine(scanX, 2, 49, SSD1306_WHITE);
    }
  }

  // 4) Yükleme çubuğu + imza (1900 ms sonrası)
  if (t >= 1900) {
    unsigned long p = t - 1900;
    int w = (p >= 1300) ? 128 : (int)(p * 128UL / 1300UL);
    display.fillRect(0, 53, w, 2, SSD1306_WHITE);

    uint8_t n = (p >= 1100) ? 15 : (uint8_t)(p * 15UL / 1100UL);
    display.setTextSize(1);
    display.setCursor(19, 56);
    for (uint8_t i = 0; i < n; i++) display.write(pgm_read_byte(&SPLASH_SIG[i]));
  }

  display.display();
}

void runSplash() {
  unsigned long t0 = millis(), t;
  while ((t = millis() - t0) < SPLASH_MS) drawSplashFrame(t);
  display.invertDisplay(true);  delay(80);   // son flaş
  display.invertDisplay(false); delay(250);
  display.clearDisplay();
  display.display();
}

// ================= MENÜ KONTROLÜ =================
void handleNavigation() {
  if (menuState == STATE_MAIN) {
    if (btnOkPressed) menuState = STATE_MENU;

  } else if (menuState == STATE_MENU) {
    if (btnUpPressed) {
      if (currentMenuIndex == 0) currentMenuIndex = TOTAL_MENU_ITEMS - 1;
      else currentMenuIndex--;
    }
    if (btnDwnPressed) {
      currentMenuIndex++;
      if (currentMenuIndex >= TOTAL_MENU_ITEMS) currentMenuIndex = 0;
    }
    if (btnOkPressed) {
      if (currentMenuIndex == IDX_EXIT)       menuState = STATE_MAIN;
      else if (currentMenuIndex == IDX_FACTORY)    menuState = STATE_CONFIRM;
      else if (currentMenuIndex == IDX_INFO)  menuState = STATE_INFO;
      else if (currentMenuIndex == IDX_TEST)  menuState = STATE_TEST;
      else                                    menuState = STATE_EDIT;
    }
    if (btnBckPressed) menuState = STATE_MAIN;

  } else if (menuState == STATE_EDIT) {
    int step = 10;
    if (btnUpPressed) {
      switch (currentMenuIndex) {
        case 0:  systemEnabled = !systemEnabled; break;
        case 1:  cycleSensorPin(+1); initSensors(); break;
        case 2:  config.parkThreshold = constrain(config.parkThreshold + step, 10, 1000); break;
        case 3:  config.kisaThreshold = constrain(config.kisaThreshold + step, 10, 1000); break;
        case 4:  config.hysteresis = constrain(config.hysteresis + 5, 0, 200); break;
        case 5:  config.parkOnDelay  = (config.parkOnDelay <= 29750) ? config.parkOnDelay + 250 : 30000; break;
        case 6:  config.parkOffDelay = (config.parkOffDelay <= 29750) ? config.parkOffDelay + 250 : 30000; break;
        case 7:  config.kisaOnDelay  = (config.kisaOnDelay <= 29750) ? config.kisaOnDelay + 250 : 30000; break;
        case 8:  config.kisaOffDelay = (config.kisaOffDelay <= 29750) ? config.kisaOffDelay + 250 : 30000; break;
        case 9:  config.drlDayPwm  = constrain(config.drlDayPwm + 5, 0, 100); break;
        case 10: config.drlFullLdr = constrain(config.drlFullLdr + step, 10, 1000); break;
        case 11: config.drlKisaPwm = constrain(config.drlKisaPwm + 5, 0, 100); break;
        case 12: config.screenSaver = (config.screenSaver + 1) % 4; break;
        case 13: config.sensorSpeed = (config.sensorSpeed + 1) % 5; break;
        case 14: config.bootDelaySec = constrain(config.bootDelaySec + 1, 1, 15); break;
      }
    }
    if (btnDwnPressed) {
      switch (currentMenuIndex) {
        case 0:  systemEnabled = !systemEnabled; break;
        case 1:  cycleSensorPin(-1); initSensors(); break;
        case 2:  config.parkThreshold = constrain(config.parkThreshold - step, 10, 1000); break;
        case 3:  config.kisaThreshold = constrain(config.kisaThreshold - step, 10, 1000); break;
        case 4:  config.hysteresis = constrain(config.hysteresis - 5, 0, 200); break;
        case 5:  config.parkOnDelay  = (config.parkOnDelay >= 250) ? config.parkOnDelay - 250 : 0; break;
        case 6:  config.parkOffDelay = (config.parkOffDelay >= 250) ? config.parkOffDelay - 250 : 0; break;
        case 7:  config.kisaOnDelay  = (config.kisaOnDelay >= 250) ? config.kisaOnDelay - 250 : 0; break;
        case 8:  config.kisaOffDelay = (config.kisaOffDelay >= 250) ? config.kisaOffDelay - 250 : 0; break;
        case 9:  config.drlDayPwm  = constrain(config.drlDayPwm - 5, 0, 100); break;
        case 10: config.drlFullLdr = constrain(config.drlFullLdr - step, 10, 1000); break;
        case 11: config.drlKisaPwm = constrain(config.drlKisaPwm - 5, 0, 100); break;
        case 12: config.screenSaver = (config.screenSaver == 0) ? 3 : config.screenSaver - 1; break;
        case 13: config.sensorSpeed = (config.sensorSpeed == 0) ? 4 : config.sensorSpeed - 1; break;
        case 14: config.bootDelaySec = constrain(config.bootDelaySec - 1, 1, 15); break;
      }
    }
    if (btnOkPressed || btnBckPressed) {
      saveSettings();
      menuState = STATE_MENU;
    }

  } else if (menuState == STATE_CONFIRM) {
    if (btnOkPressed) {
      loadDefaults();
      saveSettings();
      initSensors();
      systemEnabled = true;
      menuState = STATE_MAIN;
    } else if (btnBckPressed) {
      menuState = STATE_MENU;
    }

  } else if (menuState == STATE_INFO || menuState == STATE_TEST) {
    if (btnBckPressed || btnOkPressed) menuState = STATE_MENU;
  }
}

// ================= SETUP =================
void setup() {
  pinMode(PIN_PARK, OUTPUT);
  pinMode(PIN_KISA, OUTPUT);
  pinMode(PIN_PWM, OUTPUT);

  digitalWrite(PIN_PARK, LOW);
  digitalWrite(PIN_KISA, LOW);
  analogWrite(PIN_PWM, 0);

  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DWN, INPUT_PULLUP);
  pinMode(PIN_BTN_OK, INPUT_PULLUP);
  pinMode(PIN_BTN_BCK, INPUT_PULLUP);

  readSettings();

  Wire.begin();

  Wire.setWireTimeout(3000, true);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  }
  display.clearDisplay();

  // --- AÇILIŞ ANİMASYONU ---
  runSplash();
  // -------------------------

  initSensors();
}

// ================= EKRAN KORUYUCU (yanma korumalı) =================
// Seyrek ve sürekli hareket eden görüntü + düşük kontrast: hiçbir piksel uzun süre yanık kalmaz.
void drawSaver() {
  static int x = 20, y = 12, dx = 1, dy = 1;
  static uint16_t tick = 0;
  tick++;

  display.clearDisplay();

  // Kayan yıldızlar (3 farklı hız)
  for (uint8_t i = 0; i < 16; i++) {
    uint8_t spd = 1 + (i % 3);
    int sx = 127 - (int)((tick * spd + i * 53) % 128);
    int sy = (i * 29 + 7) % 64;
    display.drawPixel(sx, sy, SSD1306_WHITE);
  }

  // Ekranda süzülen far ikonu
  x += dx; y += dy;
  if (x <= 0 || x >= 104) dx = -dx;
  if (y <= 0 || y >= 44) dy = -dy;
  display.drawBitmap(x, y, icon_kisa_on, 24, 20, SSD1306_WHITE);

  display.display();
}

// ================= LOOP =================
void loop() {
  unsigned long now = millis();

  readButtons();
  handleNavigation();

  updateSensor(now);
  processLights(now);

  // --- EKRAN KORUYUCU MANTIĞI ---
  unsigned long saverMs = 0;
  if (config.screenSaver == 1) saverMs = 60000UL;
  else if (config.screenSaver == 2) saverMs = 300000UL;
  else if (config.screenSaver == 3) saverMs = 1800000UL;

  // Sistem kapalıyken ekran koruyucu devreye girmez (uyarı görünür kalsın)
  if (saverMs > 0 && systemEnabled && menuState == STATE_MAIN) {
    unsigned long idle = now - lastButtonPress;
    if (!displayOff && idle >= saverMs) {
      displayOff = true;
      saverBlank = false;
    } else if (displayOff && !saverBlank && idle >= saverMs + SAVER_ANIM_MS) {
      saverBlank = true;                              // 2. aşama: ekran tamamen kapalı
      display.clearDisplay();
      display.display();
    }
  }

  // Ekran, sensör salvosu sürerken güncellenmez (örnek aralıkları bozulmasın)
  if (!sensorBusy && now - lastDisplayUpdate >= 100) {
    lastDisplayUpdate = now;
    if (!displayOff)       drawOled();
    else if (!saverBlank)  drawSaver();
  }
}
