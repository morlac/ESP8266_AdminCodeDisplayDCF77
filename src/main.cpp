/**
 * AdminCodeDisplayDCF77
 */

#include <Arduino.h>

#include <Streaming.h>

#include <SPI.h>

#include <DCF77.h>
#include <TimeLib.h>

#define DCF_PIN D6	         // Connection pin to DCF 77 device
DCF77 DCF = DCF77(DCF_PIN, digitalPinToInterrupt(DCF_PIN), LOW);

int led_pin = 13;

#define DISPLAY_RST   D4     // RST pin is connected to NodeMCU pin D4 (GPIO2)
#define DISPLAY_CS    D3     // CS  pin is connected to NodeMCU pin D4 (GPIO0)
#define DISPLAY_DC    D2     // DC  pin is connected to NodeMCU pin D4 (GPIO4)
// initialize U8g2 library with hardware SPI module
// SCK (CLK) ---> NodeMCU pin D5 (GPIO14)
// MOSI(DIN) ---> NodeMCU pin D7 (GPIO13)

#include <U8g2lib.h>
U8G2_ST7565_PE12864_004_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ DISPLAY_CS, /* dc=*/ DISPLAY_DC, /* reset=*/ DISPLAY_RST);

/**
 *
 */
void setup() {
  Serial.begin(115200);

  /* switch off led */
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);

  u8g2.begin();

  u8g2.setFlipMode(true);
  u8g2.enableUTF8Print();

  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_6x10_tr);	// choose a suitable font
  u8g2.drawStr(15, 9, "AdminCodeDisplay");	// write something to the internal memory
  u8g2.drawStr(29, 19, "- starting -");

  u8g2.sendBuffer();					// transfer internal memory to the display

//  strcpy(time_source, custom_time_source.getValue());

  delay(2500);

  DCF.Start();
}

time_t current_time_t;
struct tm *current_time;

char time_str[9] = {0};
char date_show_str[11] = {0};
char date_str[7] = {0};
char admin_pass[7] = {0};

/**
 *
 */
void loop() {
  time_t DCFtime = DCF.getTime(); // Check if new DCF77 time is available

  if (DCFtime != 0) {
    Serial << F("Time is updated");
    setTime(DCFtime);
  }

  current_time_t = now();
  current_time = localtime(&current_time_t);

// https://www.fluentcpp.com/2019/08/30/how-to-disable-a-warning-in-cpp/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"

  snprintf(time_str, 9, "%02d:%02d:%02d", current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
  snprintf(date_show_str, 11, "%04d/%02d/%02d", (current_time->tm_year + 1900), current_time->tm_mon + 1, current_time->tm_mday);
  snprintf(date_str, 7,"%02d%02d%02d", current_time->tm_mday, current_time->tm_mon + 1, (current_time->tm_year + 1900) % 100);

#pragma GCC diagnostic pop

  for (int i = 0; i < 6; i++) {
    char pass_char = date_str[i] + 1;

    pass_char = (pass_char > '9' ? pass_char - 10 : pass_char);

    admin_pass[5 - i] = pass_char;
  }

  //Serial << F("touchValue1: ") << (touchValue1 == HIGH ? F("HIGH") : F("LOW")) << endl;

/*
  Serial << F("Date       : ") << date_show_str << endl;
  Serial << F("Time       : ") << time_str << endl;
  Serial << F("Date-String: ") << date_str << endl;
  Serial << F("Admin-Pass : ") << admin_pass << endl;
*/

  u8g2.clearBuffer();					// clear the internal memory

  u8g2.setFont(u8g2_font_6x10_tr);	// choose a suitable font

  u8g2.drawStr(15, 9, "AdminCodeDisplay");

  u8g2.drawStr(7, 23, (String(date_show_str) + " " + String(time_str)).c_str());

  u8g2.setFont(u8g2_font_logisoso32_tn);
  u8g2.drawStr(0, 63, admin_pass);

  u8g2.sendBuffer();

  delay(100);
}

