#define LAMP 7
#define KAITEN 8
#define KAITEN_PWM 3

#include <Wire.h>

byte duty;
byte lamp_on;

void setup() {
  pinMode(LAMP, OUTPUT);
  pinMode(KAITEN, OUTPUT);

  Serial.begin(9600);
  Serial.println("KAITEN Start");

  TCCR2A = bit(WGM20) | bit(WGM21) | bit(COM2B1);   //比較一致でLow、BOTTOMでHighをOC2Bﾋﾟﾝへ出力 (非反転動作)
  TCCR2B = bit(WGM22);  //高速PWM動作, clkT2S/8 (8分周)
  OCR2A =  99;          //16MHz/(8*(1+99))=20KHz
  OCR2B =  0;

  pinMode(KAITEN_PWM, OUTPUT);

  duty = 0;
  lamp_on = false;

  i2c_init();
}





void loop() {

  KAITEN_duty(duty);

  if (lamp_on == true) {
    LAMP_on();
  } else {
    LAMP_off();
  }

}


void KAITEN_duty(byte duty)
{
  if (duty == 0) {
    KAITEN_off();
  } else {
    KAITEN_on(duty);
  }
}

void KAITEN_on(byte duty)
{
  digitalWrite(KAITEN, LOW);
  TCCR2B |= bit(CS21);
  OCR2B = duty;
}

void KAITEN_off()
{
  digitalWrite(KAITEN, HIGH);
  TCCR2B &= ~(bit(CS22) | bit(CS21) | bit(CS20));
  OCR2B = 0;
}

void LAMP_on()
{
  digitalWrite(LAMP, LOW);
}

void LAMP_off()
{
  digitalWrite(LAMP, HIGH);
}




void i2c_init()
{
  Wire.begin(0x15) ;                 // Ｉ２Ｃの初期化、自アドレスを15とする
  Wire.onRequest(requestEvent);     // マスタからのデータ取得要求のコールバック関数登録
  Wire.onReceive(receiveEvent);     // マスタからのデータ送信対応のコールバック関数登録
  Serial.println("i2c slave test");
}



// マスターからを受信
void receiveEvent(int n) {
  byte cmd, val;
  Serial.println("receiveEvent");
  if (n == 2)  {
    cmd = Wire.read();
    val = Wire.read();
    if (cmd == 0x01) {
      duty = val;
      Serial.print("duty: ");
      Serial.println(val);
    } else if (cmd == 0x02) {
      lamp_on = val;
      Serial.print("lamp_on: ");
      Serial.println(val);
    }
  }
}

// マスターからのリクエストに対するデータ送信
void requestEvent() {
  Serial.println("requestEvent");
  Wire.write(duty);
  Wire.write(lamp_on);

  Serial.print("duty: ");
  Serial.println(duty);
  Serial.print("lamp_on: ");
  Serial.println(lamp_on);
}

