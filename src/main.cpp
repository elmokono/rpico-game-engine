#include <Arduino.h>
#include <stdio.h>
#include <Tetris.h>

Engine *engine;
Tetris *tetris;

void setup()
{
  Serial.begin(115200);

  engine = new Engine();
  tetris = new Tetris(engine);

  tetris->reset();
}

void loop(void)
{  
  //joypad, gyro, pots
  tetris->process_inputs(engine->inputs());

  tetris->gameLogic();

  tetris->draw();

  // buffer to screen  
  engine->display();
  engine->sound();
  //fps
  engine->stats();
}
