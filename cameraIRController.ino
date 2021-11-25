#include <M5StickC.h>

#define LED_ON LOW
#define LED_OFF HIGH
#define IR_ON LOW
#define IR_OFF HIGH

/* M5StickC 本体の赤外線LEDは GPIO 9 */
const int IR_SEND_PIN = GPIO_NUM_9;
const int LED_SEND_PIN = M5_LED;
 
void setup() {
  M5.begin();

  pinMode(IR_SEND_PIN, OUTPUT);
  pinMode(LED_SEND_PIN, OUTPUT);
  digitalWrite(GPIO_NUM_10, LED_OFF);
  
  /* 消費電力を下げるため */
  setCpuFrequencyMhz(10);
  M5.Axp.ScreenBreath(8);
  
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setRotation(1); /* 右手でカメラに対して持った時に画面を持ち主が読める向き */
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("Nikon IRCtrl");
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(10, 30);
  M5.Lcd.println("              ML-L3 emu");
  update_battery_info();
}
  
void loop() {
  /* M5StickC 前面のボタンは BtnA, 右側面のボタンは BtnB */
  M5.update();
  if (M5.BtnA.wasPressed() != 0) {
    digitalWrite(LED_SEND_PIN, LED_ON); /* 送信確認用 */
    ReleseNikonMLL3Shutter();
    digitalWrite(GPIO_NUM_10, LED_OFF);
  }

  update_battery_info();
  delay(100);
}

/*
 * NikonのML-L3をエミュレートした赤外線信号を送信する．
 */
void ReleseNikonMLL3Shutter() {
    for(int i = 0; i < 2; i++) {
      IR_SEND(2000);
      /* 28 ms 待つ */
      delayMicroseconds(28 * 1000);
      IR_SEND(400);
      /* 1600 micro s 待つ */
      delayMicroseconds(1.58*1000);
      IR_SEND(400);
      delayMicroseconds(3.58*1000);
      IR_SEND(400);
      delayMicroseconds(63.2 * 1000);      
    }
}

/*
 * dマイクロ秒の間38kHzの周波数でduty比1/3でIR LEDを発光する．
 */
void IR_SEND(long d) {
  /* 38000 Hz で点滅させるための待ち時間（マイクロ秒） */
  const double t = 1e6 / 38000.0 / 3.0;

  long t1 = micros();
  for (long t2 = micros(); t2 - t1 < d; t2 = micros() ) {
      digitalWrite(IR_SEND_PIN, IR_ON);
      delayMicroseconds(t);
      digitalWrite(IR_SEND_PIN, IR_OFF);
      delayMicroseconds(t*2);
  }
}

/* バッテリー残量の表示を更新する． */
void update_battery_info() {
  double vbatt = M5.Axp.GetVbatData() * 1.1 / 1000;
  int charge = int((vbatt - 3.0) / 1.2 * 100);
  if(charge > 100) {
    charge = 100;
  } else if(charge < 0) {
    charge = 1;
  }

  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 50);
  M5.Lcd.printf("Batt:%3d%%", charge);
}
