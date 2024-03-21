#include <engine_core.h>

#define BOARD_WIDTH 16
#define BOARD_HEIGHT 17
#define BLOCKS_PER_PIECE 4
class Point
{
    public:
    Point();
    Point(int x, int y);
    int x, y;
};

class Piece
{
    public:
    Piece();
    Piece(int type);
    Point blocks[BLOCKS_PER_PIECE];
    int width, height;
    int type;
    int x, y;
};

class Tetris
{
    public:
    Tetris(Engine *engine_core);
    void reset();
    void process_joy(joystick_state joy);
    void process_inputs(inputs_state state);
    void gameLogic(void);
    void draw(void);
private:
    void addPiece();
    bool checkPieceCollision(); //true if collision
    bool checkLineFull(); //true if line full
    void movePiece(int direction);
    void rotatePiece(bool cw);
    bool hasBlock(int x, int y);
    Piece getCurrentPiece(); 
    int getScore();
    bool isGameOver();
    void placePiece();
    bool gameOver;
    bool gameMap[BOARD_WIDTH][BOARD_HEIGHT];
    Piece currentPiece;
    int score;
};
