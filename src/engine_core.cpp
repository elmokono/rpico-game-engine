#include <Arduino.h>
#include <Adafruit_ST7735.h> // gpu driver
#include <SPI.h>
#include <stdio.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "engine_core.h"

Adafruit_ST7735 *tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
int rgb_r, rgb_g, rgb_b;

uint lastMillis, lastMillisJoy;
bool button1Pressed = false, button2Pressed = false, button3Pressed = false;
uint millisToJoy = 100;
float xCenterAvg = 0;
float yCenterAvg = 0;
float stickXCenter = 512;
float stickYCenter = 512;
long calibrateSamples = 0;

short intFps, fps = 0;

Engine::Engine()
{
    reset();
}

void Engine::reset()
{
    canvas = new GFXcanvas16Opt(128, 128);

    tft->initR(INITR_144GREENTAB);
    tft->setRotation(0);
    tft->fillScreen(ST7735_CYAN);

    pinMode(RGB_R, OUTPUT);
    pinMode(RGB_G, OUTPUT);
    pinMode(RGB_B, OUTPUT);

    fps = intFps = 0;
    lastMillis = millis();

    // joy
    pinMode(JOY_BT, INPUT_PULLUP);
    pinMode(JOY_B1, INPUT_PULLUP);
    pinMode(JOY_B2, INPUT_PULLUP);
    pinMode(JOY_B3, INPUT_PULLUP);
    lastMillisJoy = millis();    
}
inputs_state Engine::inputs()
{
    inputs_state state;

    //joystick
    joystick_state joy;
    joy.b1 = joy.b2 = joy.b3 = joy.b1down = joy.b2down = joy.b3down = false;
    joy.x = 0;
    joy.y = 0;
    joy.novalue = true;

    if (millis() - lastMillisJoy < millisToJoy)
    {
        state.joy_state = joy;
        return state;
    }        

    lastMillisJoy = millis();

    // 0-stickXCenter-1023

    // 368

    // converts to -1/0/+1
    int x = analogRead(JOY_AX);
    joy.x = x > stickXCenter ? (x - stickXCenter) / (STICK_MAX - stickXCenter) : ((x - stickXCenter) / (stickXCenter - STICK_MIN));
    int y = analogRead(JOY_AY);
    joy.y = y > stickYCenter ? (y - stickYCenter) / (STICK_MAX - stickYCenter) : ((y - stickYCenter) / (stickYCenter - STICK_MIN));

    PinStatus b1Down = digitalRead(JOY_B1);
    PinStatus b2Down = digitalRead(JOY_B2);
    PinStatus b3Down = digitalRead(JOY_B3);

    if (b3Down == HIGH && button3Pressed)
        button3Pressed = false;
    else if (b3Down == LOW && !button3Pressed)
        joy.b3 = button3Pressed = true;

    if (b2Down == HIGH && button2Pressed)
        button2Pressed = false;
    else if (b2Down == LOW && !button2Pressed)
        joy.b2 = button2Pressed = true;

    if (b1Down == HIGH && button1Pressed)
        button1Pressed = false;
    else if (b1Down == LOW && !button1Pressed)
        joy.b1 = button1Pressed = true;

    joy.b1pressed = button1Pressed;
    joy.b2pressed = button2Pressed;
    joy.b3pressed = button3Pressed;
    joy.b1down = b1Down == LOW;
    joy.b2down = b2Down == LOW;
    joy.b3down = b3Down == LOW;
    joy.b1down = 
    joy.novalue = false;

    //gyro

    //pot

    state.joy_state = joy;
    return state;
}
void Engine::logic()
{
    
}
void Engine::display()
{   
    analogWrite(RGB_R, rgb_r * 4);
    analogWrite(RGB_G, rgb_g * 4);
    analogWrite(RGB_B, rgb_b * 4);

    tft->drawRGBBitmap(0, 0, canvas->getBuffer(), tft->height(), tft->width());
}
void Engine::sound()
{

}
void Engine::stats(void)
{
    intFps++;
    if (millis() - lastMillis >= 1000)
    {
        lastMillis = millis();
        fps = intFps;
        intFps = 0;
        Serial.println(fps);
    }
}
void Engine::rgb(ushort r, ushort g, ushort b)
{
    rgb_r = r;
    rgb_g = g;
    rgb_b = b;
}
void Engine::calibrateStick(void)
{
    int ax = analogRead(JOY_AX);
    int ay = analogRead(JOY_AY);

    xCenterAvg += ax;
    yCenterAvg += ay;

    calibrateSamples++;
    stickXCenter = xCenterAvg / calibrateSamples;
    stickYCenter = yCenterAvg / calibrateSamples;
}