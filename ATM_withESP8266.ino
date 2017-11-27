#include <Wire.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(8,9,10,11,12,13);
#define accel_module (0x53)

int adxlPin = 6;
int xyzregister = 0x32;
int x,y,z;
byte values[6];
char output[512];
String str_adxl;

int pirPin = 2;
int pir_pin_state;
String str_pir;

int door1Pin = 4;
int door1_pin_state;
String str_d1;

int door2Pin = 5;
int door2_pin_state;
String str_d2;

int heart_beat_pin = 7;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(adxlPin,INPUT);
  pinMode(pirPin,INPUT);
  pinMode(door1Pin,INPUT);
  pinMode(door2Pin,INPUT);
  pinMode(heart_beat_pin,OUTPUT);

  digitalWrite(heart_beat_pin,HIGH);
  
  lcd.begin(20,4);
  lcd.clear();
  lcd.print("Init I2C bus.....");

Wire.begin();
Wire.beginTransmission(accel_module);
Wire.write(0x2D);
Wire.write(0); //sets allbits to zero;
Wire.endTransmission();

  digitalWrite(heart_beat_pin,LOW);
  
Wire.beginTransmission(accel_module);
Wire.write(0x2D);
Wire.write(16); //goes to measure mode
Wire.endTransmission();

  digitalWrite(heart_beat_pin,HIGH);

Wire.beginTransmission(accel_module);
Wire.write(0x2D);
Wire.write(8); //come out of sleep
Wire.endTransmission();
delay(3000);

  digitalWrite(heart_beat_pin,LOW);
  
  lcd.clear();
  lcd.println("Init Esp8266...");

Serial.print("AT\r\n");
delay(1000);
  lcd.print("AT ");
Serial.print("AT+RST\r\n");
delay(5000);
digitalWrite(heart_beat_pin,HIGH);
  lcd.print("AT+RST ");
Serial.print("AT+CWJAP=\"GST\",\"qwerty123\"\r\n");
delay(10000);
  lcd.print("AT+CWJAP ");
}

void loop() {

lcd.clear();

digitalWrite(heart_beat_pin, !digitalRead(heart_beat_pin));

update_accelerometer_data();
update_pir_data();
update_door_data();


str_adxl = "[" + (String)x + ":" + (String)y + ":" + (String)z + "]";
str_pir = (String)pir_pin_state;
str_d1 = (String)door1_pin_state;
str_d2 = (String)door2_pin_state;

Serial.print("AT+CIPSTART=\"TCP\",\"192.168.43.139\",80\r\n");
delay(500);
Serial.print("AT+CIPSEND=105\r\n");
delay(500);
digitalWrite(heart_beat_pin, !digitalRead(heart_beat_pin));
Serial.print("GET /write.php?var_adxl=" + str_adxl + "&var_pir=" + str_pir + "&var_d1=" + str_d1 + "&var_d2=" + str_d2 + " HTTP/1.1\r\nHost: 192.168.43.139\r\n\r\n");
delay(3000);
}

void update_accelerometer_data()
{
Wire.beginTransmission(accel_module);
Wire.write(xyzregister);
Wire.endTransmission();

Wire.beginTransmission(accel_module);
Wire.requestFrom(accel_module,6);

int i = 0;
while(Wire.available())
{
  values[i] = Wire.read();
  i++;
}
Wire.endTransmission();

x = (((int)values[1]) << 8) | values[0];
y = (((int)values[3]) << 8) | values[2];
z = (((int)values[5]) << 8) | values[4];

sprintf(output,"%d %d %d",x,y,z);

lcd.setCursor(0,0);
lcd.print("Accelerometer data: ");
lcd.setCursor(0,1);
lcd.print(output);
}

void update_pir_data()
{
  
  pir_pin_state = digitalRead(pirPin);

  lcd.setCursor(0,2);
  lcd.print("PIR sensor: ");
  lcd.print(pir_pin_state);
  
}
void update_door_data()
{
  door1_pin_state = digitalRead(door1Pin);
  door2_pin_state = digitalRead(door2Pin);

  lcd.setCursor(0,3);
  lcd.print("Door1- ");
  lcd.print(door1_pin_state);
  lcd.print(" Door2- ");
  lcd.print(door2_pin_state);
}

