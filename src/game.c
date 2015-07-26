#include <conio.h>
#include "neslib.h"
#include "level.h"


//LEVEL INFO
#define SCREEN_WIDTH    256
#define SCREEN_HEIGHT   240
#define ATTRIBUTE_TABLE_A 0x23C0

const unsigned char spritesPal[16] = {
    0x0C, 0x25, 0x24, 0x34,
    0x0C, 0x19, 0x29, 0x39,
    0x0C, 0x1C, 0x2C, 0x3C,
    0x0C, 0x14, 0x24, 0x34
};

const unsigned char bgPal[16] = {
    0x0C, 0x25, 0x24, 0x34,
    0x0C, 0x19, 0x29, 0x39,
    0x0C, 0x1C, 0x2C, 0x3C,
    0x0C, 0x14, 0x24, 0x34
};

const unsigned char levelBoundaries[4] = {
    8, 6, 23, 21
};

static unsigned char levelNamRef[1024];


#define EMPTY_SPRITE        0x00

//SNAKE INFO
#define SNAKE_SPRITE_SIZE   8
#define SNAKE_SPRITE        0x40
#define MAX_SNAKE_SIZE      35
#define SnakeHead           snakeCoords[0]

static unsigned char snakeCoords[MAX_SNAKE_SIZE][2];
static unsigned char snakeCleanCoords[2];
static unsigned char snakeSize;
static unsigned char x, y;



// PILLS INFO
#define PILL_SPRITE_SIZE    8
#define PILL_SPRITE         0x45
#define PILL_PALETTE        2
#define MAX_PILLS           8

static unsigned char pillsLive = 0;
static unsigned char pillsPositions[MAX_PILLS][2];


// GAME INFO
#define MAX_LEVEL 10

#define SCORE_PALETTE   0
#define SCORE_X_COORDS  8 * 21
#define SCORE_Y_COORDS  8 * 3

#define LEVEL_PALETTE   0
#define LEVEL_X_COORDS  8 * 18
#define LEVEL_Y_COORDSS  8 * 24

const static unsigned char pointsPerLevel[MAX_LEVEL] = {
    2, 5, 10, 20, 30, 40, 50, 100, 200, 500
};

const static unsigned char pillsPerLevel[MAX_LEVEL] = {
    2, 2, 3, 3, 4, 4, 5, 6, 7, 8
};
const static unsigned char speed[MAX_LEVEL] = {
    15, 12, 10, 9, 8, 8, 7, 6, 5, 4
};  
static unsigned int score = 0;
static unsigned char scoreParts[4];

static unsigned char level = 0;
static unsigned char pad;
static unsigned char gameover = 1;
static unsigned char i, j, k, l, m;
static unsigned char oamBuffer;
static unsigned char vFrameCount;
static unsigned char playing;
static unsigned char snakeVramBuffer[MAX_SNAKE_SIZE * 3 + 1];

void reset()
{
    vFrameCount = 0;
    snakeSize = 1;
    SnakeHead[0] = 16;
    SnakeHead[1] = 15;
    x = 1;
    y = 0;
    level = -1;
    pillsLive = 0;
    score = 0;
    scoreParts[0] = 0;
    scoreParts[1] = 0;
    scoreParts[2] = 0;
    scoreParts[3] = 0;
    gameover = 1;
    score = 0;
}


void drawUI()
{
    //Draw score
    oamBuffer = oam_spr(SCORE_X_COORDS, SCORE_Y_COORDS, 0x10 + scoreParts[0], SCORE_PALETTE, oamBuffer);
    oamBuffer = oam_spr(SCORE_X_COORDS + 8, SCORE_Y_COORDS, 0x10 + scoreParts[1], SCORE_PALETTE, oamBuffer);
    oamBuffer = oam_spr(SCORE_X_COORDS + 16, SCORE_Y_COORDS, 0x10 + scoreParts[2], SCORE_PALETTE, oamBuffer);
    oamBuffer = oam_spr(SCORE_X_COORDS + 24, SCORE_Y_COORDS, 0x10 + scoreParts[3], SCORE_PALETTE, oamBuffer);

    //Draw level
    k = (level == 0xFF) ? 0 : level + 1; 
    i = k / 10;
    j = k - i * 10;
    oamBuffer = oam_spr(LEVEL_X_COORDS, LEVEL_Y_COORDSS, 0x10 + i, LEVEL_PALETTE, oamBuffer);
    oamBuffer = oam_spr(LEVEL_X_COORDS + 8, LEVEL_Y_COORDSS, 0x10 + j, LEVEL_PALETTE, oamBuffer);
}


void enter()
{
    /*
    INPUT CONTROL
    */
    reset();
    pad = pad_trigger(0);
    if ((pad & PAD_START)
        || (pad & PAD_A)
        || (pad & PAD_B)) {
        gameover = 0;
    }

    oamBuffer = 0;
    oam_clear();

    i = 116;
    j = 100;
    oamBuffer = oam_spr(i + 8 * 0, j, 'G' - 0x20, SCORE_PALETTE, oamBuffer);
    oamBuffer = oam_spr(i + 8 * 1, j, 'A' - 0x20, SCORE_PALETTE, oamBuffer);
    oamBuffer = oam_spr(i + 8 * 2, j, 'M' - 0x20, SCORE_PALETTE, oamBuffer);
    oamBuffer = oam_spr(i + 8 * 3, j, 'E' - 0x20, SCORE_PALETTE, oamBuffer);
    j += 8;
    oamBuffer = oam_spr(i + 8 * 0, j, 'O' - 0x20, SCORE_PALETTE, oamBuffer);
    oamBuffer = oam_spr(i + 8 * 1, j, 'V' - 0x20, SCORE_PALETTE, oamBuffer);
    oamBuffer = oam_spr(i + 8 * 2, j, 'E' - 0x20, SCORE_PALETTE, oamBuffer);
    oamBuffer = oam_spr(i + 8 * 3, j, 'R' - 0x20, SCORE_PALETTE, oamBuffer);
    
    drawUI();
}

void game()
{
    /*
    INPUT CONTROL
    */

    pad = pad_trigger(0);
    if (pad & PAD_LEFT) {
        gameover = snakeSize > 1 && (x == 1);
        x = -1;
        y = 0;
    } else if (pad & PAD_RIGHT) {
        gameover = snakeSize > 1 && (x == 0xFF);
        x = 1;
        y = 0;
    } else if (pad & PAD_UP) {
        gameover = snakeSize > 1 && (y == 1);
        x = 0;
        y = -1;
    } else if (pad & PAD_DOWN) {
        gameover = snakeSize > 1 && (y == 0xFF);
        x = 0;
        y = 1;
    }

    /**
    GAMELOOP CODE
    **/

    //Spawn pills
    if (pillsLive == 0) {
        ++level;
        level = level > MAX_LEVEL ? MAX_LEVEL : level;
        for (i = 0; i < pillsPerLevel[level]; ++i) {
            j = rand8() % (levelBoundaries[2] - levelBoundaries[0]);
            k = rand8() % (levelBoundaries[3] - levelBoundaries[1]);
            pillsPositions[i][0] = levelBoundaries[0] + j;
            pillsPositions[i][1] = levelBoundaries[1] + k;
            ++pillsLive;
        }
    //Eat pill
    } else {
        for (i = 0; i < MAX_PILLS; i++) {  
            j = pillsPositions[i][0];
            k = pillsPositions[i][1];
            if (SnakeHead[0] == j && SnakeHead[1] == k) {
                pillsPositions[i][0] = -1;
                pillsPositions[i][1] = -1;
                --pillsLive;
                
                //Grow snake
                j = snakeSize;
                ++snakeSize;
                snakeSize = (snakeSize > MAX_SNAKE_SIZE - 1)? MAX_SNAKE_SIZE - 1 : snakeSize;
                if (j != snakeSize) {
                    snakeCoords[snakeSize - 1][0] = snakeCoords[snakeSize - 2][0];
                    snakeCoords[snakeSize - 1][1] = snakeCoords[snakeSize - 2][1];
                }
                score += pointsPerLevel[level];
                scoreParts[0] = score / 1000;
                scoreParts[1] = score / 100 - scoreParts[0] * 10;
                scoreParts[2] = score / 10 - (scoreParts[0] * 100 + scoreParts[1] * 10);
                scoreParts[3] = score - (scoreParts[0] * 1000 + scoreParts[1] * 100 + scoreParts[2] * 10);
            }
        }
    }

    //Move snake
    i = vFrameCount / speed[level] * speed[level];
    if (i == vFrameCount) {
        snakeCleanCoords[0] = snakeCoords[snakeSize - 1][0];
        snakeCleanCoords[1] = snakeCoords[snakeSize - 1][1];
        
        for (i = snakeSize - 1; i > 0; --i) {
            snakeCoords[i][0] = snakeCoords[i-1][0];
            snakeCoords[i][1] = snakeCoords[i-1][1];
        }
        
        SnakeHead[0] += x;
        SnakeHead[1] += y;
    }


    //Check gameover
    gameover = gameover || (SnakeHead[0] < levelBoundaries[0]);
    gameover = gameover || (SnakeHead[0] > levelBoundaries[2]);
    gameover = gameover || (SnakeHead[1] < levelBoundaries[1]);
    gameover = gameover || (SnakeHead[1] > levelBoundaries[3]);
    for (i = 2; i < snakeSize; i++) {
        gameover = gameover || (snakeCoords[i][0] == SnakeHead[0] &&  snakeCoords[i][1] == SnakeHead[1]);
        if (gameover) {
            break;
        }
    }

    /***
    DRAW CODE
    **/

    oam_clear();
    oamBuffer = 0;

    //Draw snake code
    l = gameover ? EMPTY_SPRITE : SNAKE_SPRITE;
    i = gameover ? 1 : 0;
    for (i; i < snakeSize; ++i) {
        j = snakeCoords[i][0];
        k = snakeCoords[i][1];
        snakeVramBuffer[i * 3 + 0] = MSB(NTADR_A(j, k));
        snakeVramBuffer[i * 3 + 1] = LSB(NTADR_A(j, k));
        snakeVramBuffer[i * 3 + 2] = l;    
    }

    //Clean empty zones
    j = snakeCleanCoords[0];
    k = snakeCleanCoords[1];
    snakeVramBuffer[i * 3 + 0] = MSB(NTADR_A(j, k));
    snakeVramBuffer[i * 3 + 1] = LSB(NTADR_A(j, k));
    snakeVramBuffer[i * 3 + 2] = EMPTY_SPRITE;

    snakeVramBuffer[(++i) * 3 + 0] = NT_UPD_EOF;

    set_vram_update(snakeVramBuffer);


    //Draw pills
    for (i = 0; i < pillsPerLevel[level]; ++i) {
        if (pillsPositions[i][0] != -1 && pillsPositions[i][1] != -1) {
            oamBuffer = oam_spr(pillsPositions[i][0] * 8, pillsPositions[i][1] * 8, PILL_SPRITE, PILL_PALETTE, oamBuffer);
        }
    }

    drawUI();
}


void main(void)
{
    pal_spr(spritesPal);
    pal_bg(bgPal);

    //TODO: remove copy by packing level_nam contents
    memcpy(levelNamRef, level_nam, 1024);
    
    vram_adr(NAMETABLE_A);
    vram_write(levelNamRef, 1024);
    
    oam_clear();
    ppu_on_all();

    while(1)
    {
        ppu_wait_frame();
        vFrameCount++;

        if (gameover) {
            enter();
        } else {
            game();
        }
    }
}