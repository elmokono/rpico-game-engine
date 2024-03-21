#include <Arduino.h>
#include <stdlib.h>
#include <time.h>

#include <sprites.h>
#include "Tetris.h"

#define STAGE_TITLE_SCREEN 0
#define STAGE_INGAME 1
#define STAGE_GAMEOVER 2

#define MAGENTA 0xF81F

#define PRESS_ANY_KEY "-PRESS ANY KEY-"

Engine *engine_core_ref;

uint lastMillisMovePiece, lastMillisTitleScreen;
bool titleScreenOn = true;
uint millisToMovePiece = 500;
uint millisToTitleScreen = 700;
int currentStage = STAGE_TITLE_SCREEN;
bool gameOver;
bool gameMap[BOARD_WIDTH][BOARD_HEIGHT];
Piece currentPiece;
int score;

Point::Point(int x, int y)
{
    this->x = x;
    this->y = y;
}

Point::Point()
{
    this->x = 0;
    this->y = 0;
}

Piece::Piece()
{
    srand(time(NULL));
    Piece(rand() % 7);
}

Piece::Piece(int type)
{
    this->type = type;
    switch (type)
    {
    case 0: //'T':
        blocks[0] = Point(1, 0);
        blocks[1] = Point(0, 1);
        blocks[2] = Point(1, 1);
        blocks[3] = Point(2, 1);
        break;
    case 1: //'L':
        blocks[0] = Point(0, 1);
        blocks[1] = Point(1, 1);
        blocks[2] = Point(2, 0);
        blocks[3] = Point(2, 1);
        break;
    case 2: //'I':
        blocks[0] = Point(0, 1);
        blocks[1] = Point(1, 1);
        blocks[2] = Point(2, 1);
        blocks[3] = Point(3, 1);
        break;
    case 3: //'J':
        blocks[0] = Point(0, 0);
        blocks[1] = Point(0, 1);
        blocks[2] = Point(1, 1);
        blocks[3] = Point(2, 1);
        break;
    case 4: //'S':
        blocks[0] = Point(1, 0);
        blocks[1] = Point(2, 0);
        blocks[2] = Point(0, 1);
        blocks[3] = Point(1, 1);
        break;
    case 5: //'Z':
        blocks[0] = Point(0, 0);
        blocks[1] = Point(1, 0);
        blocks[2] = Point(1, 1);
        blocks[3] = Point(2, 1);
        break;
    case 6: //'O':
        blocks[0] = Point(0, 0);
        blocks[1] = Point(1, 0);
        blocks[2] = Point(0, 1);
        blocks[3] = Point(1, 1);
        break;
    default:
        break;
    }

    width = 0;
    height = 0;
    for (int i = 0; i < BLOCKS_PER_PIECE; i++)
    {
        if (blocks[i].x > width)
            width = blocks[i].x;
        if (blocks[i].y > height)
            height = blocks[i].y;
    }
}

Piece Tetris::getCurrentPiece()
{
    return currentPiece;
}

Tetris::Tetris(Engine *engine_core)
{
    engine_core_ref = engine_core;
    reset();
}

bool Tetris::isGameOver()
{
    return gameOver;
}

void Tetris::reset()
{
    score = 0;
    gameOver = false;

    for (int i = 0; i < BOARD_WIDTH; i++)
        for (int j = 0; j < BOARD_HEIGHT; j++)
            gameMap[i][j] = (j == (BOARD_HEIGHT - 1)) ? true : false;

    lastMillisMovePiece = millis();
    lastMillisTitleScreen = millis();

    this->addPiece();
}

void Tetris::process_joy(joystick_state joy)
{
  if (joy.novalue)
    return;

  if (currentStage == STAGE_TITLE_SCREEN)
  {
    engine_core_ref->calibrateStick();
  }

  // control tetris
  if (currentStage == STAGE_INGAME)
  {
    if (joy.x >= 0.8)
      this->movePiece(0);
    else if (joy.x <= -0.8)
      this->movePiece(1);
    // else if (joy.y >= 0.8)
    //   tetrisCore->movePiece(2);
  }

  if (joy.b1)
  {
    if (currentStage == STAGE_INGAME)
    {
      this->rotatePiece(2);
    }
    if (currentStage == STAGE_GAMEOVER)
    {
      reset();
      currentStage = STAGE_INGAME;
    }
    if (currentStage == STAGE_TITLE_SCREEN)
    {
      reset();
      currentStage = STAGE_INGAME;
      engine_core_ref->rgb(0, 0, 0);
    }
  }

  if (joy.b2down)
  {
    if (currentStage == STAGE_INGAME)
    {
      this->movePiece(2);
    }
  }

  if (joy.b2)
  {
    if (currentStage == STAGE_GAMEOVER)
    {
      reset();
      currentStage = STAGE_INGAME;
    }
    if (currentStage == STAGE_TITLE_SCREEN)
    {
      reset();
      currentStage = STAGE_INGAME;
      engine_core_ref->rgb(0, 0, 0);
    }
  }
}

void Tetris::process_inputs(inputs_state state)
{
  process_joy(state.joy_state);
}

void Tetris::gameLogic(void)
{
  if (currentStage == STAGE_TITLE_SCREEN)
  {
    if (millis() - lastMillisTitleScreen < millisToTitleScreen)
      return;
    lastMillisTitleScreen = millis();
    titleScreenOn = !titleScreenOn;
    if (titleScreenOn)
      engine_core_ref->rgb(0, 0, 255);
    else
      engine_core_ref->rgb(0, 0, 0);
  }

  if (currentStage == STAGE_INGAME)
  {
    // move piece down
    if (millis() - lastMillisMovePiece < millisToMovePiece)
      return;

    lastMillisMovePiece = millis();

    this->movePiece(2); // down

    if (isGameOver())
    {
      currentStage = STAGE_GAMEOVER;
    }
  }
}

void Tetris::draw(void)
{
  if (currentStage == STAGE_TITLE_SCREEN)
  {
    engine_core_ref->canvas->fillBitmap(title_screen);

    if (titleScreenOn)
      engine_core_ref->canvas->print(4, 96, (char *)PRESS_ANY_KEY, MAGENTA);
  }

  if (currentStage == STAGE_INGAME || currentStage == STAGE_GAMEOVER)
  {
    // background
    engine_core_ref->canvas->fillBitmap(bgImage, MAGENTA);

    // sprites
    for (int i = 0; i < BOARD_WIDTH; i++)
      for (int j = 0; j < (BOARD_HEIGHT - 1); j++)
        if (this->hasBlock(i, j))
          engine_core_ref->canvas->drawRGBBitmap(i * 8, j * 8, block_still, 8, 8);

    if (currentStage == STAGE_INGAME)
    {
      Piece currentPiece = getCurrentPiece();
      for (int i = 0; i < BLOCKS_PER_PIECE; i++)
        engine_core_ref->canvas->drawRGBBitmap(
            (currentPiece.x + currentPiece.blocks[i].x) * 8,
            (currentPiece.y + currentPiece.blocks[i].y) * 8,
            block, 8, 8, MAGENTA);
    }

    // fonts
    engine_core_ref->canvas->print(2, 2, (char *)"Score", MAGENTA);
    char score_value[7];
    snprintf(score_value, sizeof(score_value), "%06d", this->getScore());
    engine_core_ref->canvas->print(78, 2, score_value, MAGENTA);
  }

  if (currentStage == STAGE_GAMEOVER)
  {
    engine_core_ref->canvas->drawRGBBitmap(0, 48, gameover, 128, 32, MAGENTA);
  }
}

int Tetris::getScore()
{
    return score;
}

bool Tetris::hasBlock(int x, int y)
{
    return gameMap[x][y];
}

void Tetris::addPiece()
{
    // start position
    srand(time(NULL));
    currentPiece = Piece(rand() % 7);
    currentPiece.x = 6;
    currentPiece.y = 0;

    if (checkPieceCollision())
        gameOver = true;
}

void Tetris::movePiece(int direction)
{
    // check if the piece can move
    switch (direction)
    {
    case 0: // left
        currentPiece.x--;
        break;
    case 1: // right
        currentPiece.x++;
        break;
    case 2: // down
        currentPiece.y++;
        break;
    default:
        break;
    }

    if (checkPieceCollision())
    {
        // move back
        switch (direction)
        {
        case 0: // x--
            currentPiece.x++;
            break;
        case 1: // x++
            currentPiece.x--;
            break;
        case 2: // y++
            currentPiece.y--;
            placePiece();
            checkLineFull();
            addPiece();
            break;
        default:
            break;
        }
    }
}

void Tetris::placePiece()
{
    for (int i = 0; i < BLOCKS_PER_PIECE; i++)
        gameMap[currentPiece.blocks[i].x + currentPiece.x][currentPiece.blocks[i].y + currentPiece.y] = true;
}

bool Tetris::checkLineFull()
{
    int rowsFull = 0;
    bool full = true;
    for (int j = BOARD_HEIGHT - 2; j > -1; j--)
    {
        full = true;
        for (int i = 0; i < BOARD_WIDTH; i++)
            if (!gameMap[i][j])
            {
                full = false;
                break;
            }

        if (full)
        {
            for (int i = 0; i < BOARD_WIDTH; i++)
                gameMap[i][j] = false;
            for (int i = j; i > 0; i--)
                for (int k = 0; k < BOARD_WIDTH; k++)
                    gameMap[k][i] = gameMap[k][i - 1];
            j++;
            rowsFull++;
        }
    }

    if (rowsFull > 0)
        score += rowsFull * rowsFull * 10;

    return true;
}

bool Tetris::checkPieceCollision()
{
    for (int i = 3; i > -1; i--)
        if (currentPiece.blocks[i].x + currentPiece.x < 0 || currentPiece.blocks[i].x + currentPiece.x > 15)
            return true;

    // start checking from the right/bottom (speedup)
    int bottomCheck = currentPiece.y + currentPiece.height + 1;
    if (bottomCheck > BOARD_HEIGHT)
        bottomCheck = BOARD_HEIGHT;

    int rightCheck = currentPiece.x + currentPiece.width + 1;
    if (rightCheck > BOARD_WIDTH)
        rightCheck = BOARD_WIDTH;

    for (int x = (currentPiece.x == 0 ? 0 : (currentPiece.x - 1)); x < rightCheck; x++)
        for (int y = currentPiece.y; y < bottomCheck; y++)
            if (gameMap[x][y])
                for (int i = (BLOCKS_PER_PIECE - 1); i > -1; i--)
                    if (currentPiece.blocks[i].x + currentPiece.x == x && currentPiece.blocks[i].y + currentPiece.y == y)
                        return true;

    return false;
}

void Tetris::rotatePiece(bool cw)
{
    // see https://medium.com/swlh/matrix-rotation-in-javascript-269cae14a124
    // convert piece to a matrix
    int size = (currentPiece.width > currentPiece.height ? currentPiece.width : currentPiece.height) + 1;

    int matrix[size][size];
    int destination[size][size];

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            matrix[i][j] = destination[i][j] = 0;

    for (int i = 0; i < BLOCKS_PER_PIECE; i++)
        matrix[currentPiece.blocks[i].x][currentPiece.blocks[i].y] = 1;

    // recalculate measures
    currentPiece.width = currentPiece.height = 0;
    // rotate matrix
    int n = 0;
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            destination[i][j] = cw ? matrix[size - j - 1][i] : matrix[j][size - i - 1];
            if (destination[i][j] == 1)
            {
                currentPiece.blocks[n++] = Point(i, j);
                if (i > currentPiece.width)
                    currentPiece.width = i;
                if (j > currentPiece.height)
                    currentPiece.height = j;
            }
        }

    // rollback rotation
    if (checkPieceCollision())
        rotatePiece(!cw);   
}
