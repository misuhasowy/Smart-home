
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_NODE_MCU_BOARD

/************************* Pin Definition *********************************/

//Led 1,2 dieu khien phong khach va bep
#define led1              D6 //GPIO 012
#define led2              D8 //GPIO 015

//DHT11 doc nhiet do do am
#define DHTPIN            D3

//buzzer de canh bao khi gas - tam thay bang led
#define mqled            D0

//cam bien mua + servo
#define rain              D7
#define rainservo         D5


//servo dong mo cua
#define servo             D4

int gas;
Servo myservo;
byte degree[8] = {0B01110,0B01010,0B01110,0B00000,0B00000,0B00000,0B00000,0B00000};
boolean blynkState=0;

char auth[] = "Blynk_Token";

// Uncomment whatever type you're using!
#define DHTTYPE DHT11     // DHT 11

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

char ssid[] = "SSID";
char pass[] = "PASS";
// lien ket chan ao den led
  BLYNK_WRITE(V0)
{
  int pin=param.asInt();
  digitalWrite(12,pin);
  }

BLYNK_WRITE(V1)
{
  int pin=param.asInt();
  digitalWrite(15,pin);
  }
  // servo dong mo cua
  BLYNK_WRITE(V2)
{   
  int value = param.asInt(); // lay gia tri
  myservo.write(value);
  }
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  }else{
    if(blynkState==1){
      lcd.setCursor(10,0);
      lcd.print(t,1);
      lcd.write(0);
      lcd.print("C ");
      lcd.setCursor(10,1);
      lcd.print(h,0);
      lcd.print("%");
    }
     Serial.print("Nhiệt độ: "); Serial.println(t);
    Serial.print("Độ ẩm: "); Serial.println(h);
  }
  // lien ket chan ao den dht
  Blynk.virtualWrite(V3, t);
  Blynk.virtualWrite(V4, h);
  //lien ket chan ao voi Mq-08 & led
  Blynk.virtualWrite(V5, gas);

}

void setup()
{

  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass); //BlynkEdgent.begin();
//LCD
Wire.begin(14,12);         //Khởi tạo chân kết nối I2C
  lcd.init();                //Khởi tạo LCD
  lcd.clear();               //Xóa màn hình
  lcd.backlight();           //Bật đèn nền cho LCD
  lcd.createChar(0, degree);   //Tạo ký tự lưu vào byte thứ 0
  lcd.setCursor(4,0);        //Cột 2, dòng 0
  lcd.print("He thong");    //Ghi chữ bắt đầu từ cột thứ 2 dòng 0
  lcd.setCursor(0,1);        //Cột 0, dòng thứ 1
  lcd.print("canh bao nhiet!");
  delay(2000);
  lcd.clear();
  lcd.setCursor(1,0);        //Cột 2, dòng 0
  lcd.print("Dang thiet lap");    //Ghi chữ bắt đầu từ cột thứ 2 dòng 0
  lcd.setCursor(2,1);        //Cột 0, dòng thứ 1
  lcd.print("cau hinh...!");
  dht.begin();
  pinMode(mqled, OUTPUT);
  pinMode(A0, INPUT);
//chay ham ben tren theo chu ky
  timer.setInterval(1000L, sendSensor);
  myservo.attach(2);
}

void loop()
{
  //LCD
  if(Blynk.connected()){
    if(blynkState==0){
      blynkState=1;
      lcd.clear();
      lcd.setCursor(0,0);        
      lcd.print("Nhiet do: ");  
      lcd.setCursor(0,1);        
      lcd.print("Do am   : ");
    }
  }
  // chay servo + 2 led + dht
  Blynk.run();
  timer.run();
  // chay MQ-8
   gas=analogRead(A0);
  Serial.println(gas);
  
  if(gas>190)
  {
     digitalWrite(16,LOW);
    digitalWrite(5,HIGH); 
    delay(2000);  
  }
  
  if(gas<190)
  {
      digitalWrite(16,HIGH);
      digitalWrite(5,LOW);
      delay(2000);
  }
}
