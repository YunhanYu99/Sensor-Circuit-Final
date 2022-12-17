//Capactive sensor
//OLED
//Gyroscope
//count_time
//Dec.2022 Sensor & circuit Group 2-13

#include <CapacitiveSensor.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "GY521.h"
GY521 sensor(0x68);
uint32_t counter = 0;


// Define OLED
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_DC 8
#define OLED_CS 10
#define OLED_RESET 9
//D0 13
//D1 11
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         &SPI, OLED_DC, OLED_RESET, OLED_CS);



// Define capactive sensor
CapacitiveSensor cs_2_A1 = CapacitiveSensor(2, A1);  // pin A1 is sensor pin, add a wire and or foil
int in = 2;
int out = A1;
int state = HIGH;
int ar;
int r;
int p = 0;
long time_cap = 0;     //count reaction time of capactive sensor
long debounce = 2000;  //Capactive sensor reaction time 2s


//Define Time Countdown
unsigned int Time_second;                   //sec
unsigned int Time_minute;                   //min
unsigned long Time_minute_setting = 60000;  // 1min = 60000 ms
unsigned long Time_second_setting = 1000;   //1sec = 1000 ms
int mod = 0;                                //For timer output mod(mod > 900 to print on OLED)
int count_second = 0;
int count = 0;
int Min_remain = 0;
int Sec_remain = 0;
int Min_all = 0;
int Sec_all = 60;
int times_count = 0;
unsigned long start_time;
unsigned long current_time;


//Define output string
String connect = "\n";
String outputtext = " ";
String temp_connect = " C";
String temp_output = " ";
String time_connect = ":";

//Define orientation
int orientation = 2;  //1 for setup ver; 2 for parall;
long time_gyr = 0;

void setup() {
  Serial.begin(9600);
  // Serial.println();
  // Serial.println(__FILE__);
  // Serial.print("GY521_LIB_VERSION: ");
  // Serial.println(GY521_LIB_VERSION);
  //Set capactive sensor
  pinMode(A1, INPUT);
  // pinMode(A2,)
  // pinMode(7, OUTPUT);

  //Set Gyroscope
  Wire.begin();

  while (sensor.wakeup() == false) {
    Serial.print(millis());
    Serial.println("\tCould not connect to GY521");
    delay(1000);
  }
  sensor.setAccelSensitivity(0);  // 2g
  sensor.setGyroSensitivity(0);   // 250 degrees/s
  sensor.setThrottle();
  // set calibration values from calibration sketch.
  sensor.gxe = 0;
  sensor.gye = 0;
  sensor.gze = 0;


  //Set OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();

  //Set timer
  start_time = millis();
}
void loop() {
  //read capactive sensor
  ar = analogRead(A1);
  Serial.println(ar);
  if (ar > 10 && millis() - time_cap > debounce) {
    if ((state == HIGH)) {
      state = LOW;
      start_time = millis();
    } else
      state = HIGH;
    time_cap = millis();
  }
  // digitalWrite(7, state); LED to show result when test the sensor
  // p = ar;

  //Gyroscope read
  sensor.read();
  int gz = sensor.getGyroZ();
  int t = sensor.getTemperature();
  temp_output = t + temp_connect;
  if ((gz > 50) && (orientation == 1) && (millis() - time_gyr > debounce)) {
    orientation = 2;
    time_gyr = millis();
    start_time = millis();
    delay(1000);
  }
  if ((gz < -50) && (orientation == 2) && (millis() - time_gyr > debounce)) {
    orientation = 1;
    time_gyr = millis();
    start_time = millis();
    mod = 0;
    count = 0;
    times_count = 0;
    start_time = millis();
    delay(1000);
  }

  if (orientation == 1) {
    Min_all = 0;
    Sec_all = 60;
    if (state == LOW){        
      current_time = millis() - start_time;
      count_second = current_time / Time_second_setting;
      mod = (current_time - start_time) % 1000;
      if ((mod > 900) && (count_second == count + 1)){
        Time_minute = current_time / Time_minute_setting;
        Time_second = (current_time % Time_minute_setting) / Time_second_setting;
        if (Time_second == 59){
          times_count = times_count + 1;
        }
        count = count + 1;
        Min_remain = Min_all - Time_minute;
        Sec_remain = Sec_all - Time_second;
        outputtext = times_count + connect + Sec_remain;
        showtext_ver(outputtext);
      }

    }else{
      showtext_ver(temp_output);
      mod = 0;
      count = 0;
      times_count = 0;
      start_time = millis();
    }
  } else {
    Min_all = 14;
    Sec_all = 60;
      if (state == LOW){        
        current_time = millis() - start_time;
        count_second = current_time / Time_second_setting;
        mod = (current_time - start_time) % 1000;
        if ((mod > 900) && (count_second == count + 1)){
          Time_minute = current_time / Time_minute_setting;
          Time_second = (current_time % Time_minute_setting) / Time_second_setting;
          count = count + 1;
          Min_remain = Min_all - Time_minute;
          Sec_remain = Sec_all - Time_second;
          if (Sec_remain == 60){
            Sec_remain = 0;
          }
          outputtext = Min_remain + time_connect + Sec_remain;
          showtext(outputtext);
        }
      }else{
        showtext(temp_output);
        mod = 0;
        count = 0;
        times_count = 0;
        start_time = millis();
      }
  }
}


//Show vertical text
void showtext_ver(String outputtext) {
  display.clearDisplay();
  display.setRotation(3);
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println(outputtext);
  display.display();
}

//Show normal text
void showtext(String outputtext) {
  display.clearDisplay();
  display.setRotation(4);
  display.setTextSize(3.5);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println(outputtext);
  display.display();
}
