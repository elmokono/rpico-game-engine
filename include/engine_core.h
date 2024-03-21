#include <Arduino.h>
#include <Adafruit_ST7735.h> // gpu driver
#include <GFXcanvas16Opt.h> // canvas layer
#include <SPI.h>
#include <stdio.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

/*
pico spi0 pins
---------------------------
#define PIN_SPI_MISO  (16u)
#define PIN_SPI_MOSI  (19u)
#define PIN_SPI_SCK   (18u)
#define PIN_SPI_SS    (17u)
*/

#define TFT_DC 21
#define TFT_CS 17
#define TFT_RST 20

#define JOY_AX 26
#define JOY_AY 27
#define JOY_BT 22

#define JOY_B1 8
#define JOY_B2 9
#define JOY_B3 10

#define RGB_R 2
#define RGB_G 3
#define RGB_B 4

#define STICK_MIN 0.0
#define STICK_MAX 1023.0

struct joystick_state
{
  float x;
  float y;
  bool novalue;
  bool b1;
  bool b2;
  bool b3;
  bool b1down;
  bool b2down;
  bool b3down;
  bool b1pressed;
  bool b2pressed;
  bool b3pressed;
};
struct inputs_state
{
  joystick_state joy_state;
};

class Engine
{
public:
    Engine();
    void reset(void);
    inputs_state inputs(void);
    void logic(void);
    void rgb(ushort r, ushort g, ushort b);
    void display(void);
    void sound(void);
    void stats(void);
    void calibrateStick(void);
    GFXcanvas16Opt *canvas;
private:
};