#include <ArduinoOSCWiFi.h>
#include <M5StickCPlus.h>

const char *ssid = "";
const char *pass = "";
const int bind_port = 10001;
const int send_port = 10002;

#define SAMPLE_PERIOD 20    // サンプリング間隔(ミリ秒)
#define SAMPLE_SIZE 150     // 20ms x 150 = 3秒

// 水平静止で重力加速度1000mGが常にかかることを考慮する
#define MINZ -1000  // 縦軸の最小値 mG
#define MAXZ 3000  // 縦軸の最大値 mG

IPAddress ip(192, 168, 11, 101);           // for fixed IP Address
IPAddress gateway(192,168, 11, 1);        //
IPAddress subnet(255, 255, 255, 0);      //

int flag;

#define BTN_A_PIN 37
#define BTN_B_PIN 39
#define LED_PIN   10

// このLEDは、GPIO10の電位を下げることで発光するタイプ
#define LED_ON  LOW
#define LED_OFF HIGH

// INPUT_PULLUPが有効かは不明だが、有効という前提で定義
#define BTN_ON  LOW
#define BTN_OFF HIGH

uint8_t prev_btn_a = BTN_OFF;
uint8_t btn_a      = BTN_OFF;
uint8_t prev_btn_b = BTN_OFF;
uint8_t btn_b      = BTN_OFF;
bool btn_state = false;

void setup() {

  // initialize the M5StickC object
  M5.begin();
  M5.Axp.ScreenBreath(10); // 画面の明るさ7〜１2
  M5.Lcd.setRotation(3); // 画面を横向きにする
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(5, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.printf("Start!!");

  // button & led
  pinMode(BTN_A_PIN, INPUT_PULLUP);
  pinMode(BTN_B_PIN, INPUT_PULLUP);
  pinMode(LED_PIN,   OUTPUT);
  digitalWrite(LED_PIN, LED_OFF);

  // 加速度センサ
  M5.IMU.Init();
  M5.IMU.SetAccelFsr(M5.IMU.AFS_4G);
  
  // WiFi stuff
  WiFi.begin(ssid, pass);
  WiFi.config(ip, gateway, subnet);

  // 受信したOSCパケットを直接変数にバインドします
  flag = 0;
  OscWiFi.subscribe(bind_port, "/test", flag);


}


void loop() {
  OscWiFi.update(); // 自動的に送受信するために必須

  // check button
  btn_a = digitalRead(BTN_A_PIN);
  btn_b = digitalRead(BTN_B_PIN);
  
  if(prev_btn_a == BTN_OFF && btn_a == BTN_ON)
  {
    btn_state = !btn_state;
    delay(200); // チャタリング防止
    
    if(btn_state){
      digitalWrite(LED_PIN, LED_ON);
    }else{
      digitalWrite(LED_PIN, LED_OFF);
    }

  }
  
  float ax, ay, az;  // 加速度データを読み出す変数
  M5.IMU.getAccelData(&ax,&ay,&az);
  
  if(btn_state)
  {
    OscWiFi.send("192.168.11.21", send_port, "/acc", ax, ay, az);
  }

  delay(SAMPLE_PERIOD);
}
