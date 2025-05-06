#ifndef HW_H
#define HW_H
#include <Arduino.h>

#define SDA_PIN PB11
#define SCL_PIN PB10

#define OLED_I2C_ADDRESS 0x3C

#define HX711_SCK_PIN PB4
#define HX711_DOUT_PIN PB3

#define BUZZER_PIN PA4

// 需要先把CTL输出高电平，然后才可以读取 SIG_PIN的电压
#define VMETER_CTL_PIN PA6
#define VMETER_SIG_PIN PA5

#define BTN_OK_PIN PA3
#define BTN_NEXT_PIN PA2
#define BTN_PREV_PIN PA1
#define BTN_CANCEL_PIN PA0

#endif