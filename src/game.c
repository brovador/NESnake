#include <conio.h>
#include "neslib.h"
#include "level.h"


//LEVEL INFO
#define SCREEN_WIDTH    256
#define SCREEN_HEIGHT   240
#define ATTRIBUTE_TABLE_A 0x23C0

const unsigned char spritesPal[16] = {
    0x0F, 0x11, 0x21, 0x31,
    0x0F, 0x12, 0x22, 0x32,
    0x0F, 0x13, 0x23, 0x33,
    0x0F, 0x14, 0x24, 0x34
};

const unsigned char bgPal[16] = {
    0x0F, 0x25, 0x24, 0x34,
    0x0F, 0x2A, 0x22, 0x32,
    0x0F, 0x13, 0x23, 0x33,
    0x0F, 0x14, 0x24, 0x34
};

const unsigned char levelBoundaries[4] = {
    8, 6, 23, 21
};

static unsigned char levelNamRef[1024];


#define EMPTY_SPRITE        0x00

//SNAKE INFO
#define SNAKE_PALETTE       0
#define SNAKE_SPRITE_SIZE   8
#define SNAKE_SPRITE        0x40
#define SNAKE_PALETTE       0
#define MAX_SNAKE_SIZE      35
#define SnakeHead           snakeCoords[0]

static unsigned char snakeCoords[MAX_SNAKE_SIZE][2];
static unsigned char snakeCleanCoords[2];
static unsigned char snakeSize;
static unsigned char x, y;



// PILLS INFO
#define PILL_SPRITE_SIZE    8
#define PILL_SPRITE         0x40
#define PILL_PALETTE        0
#define MAX_PILLS           8

static unsigned char pillsLive = 0;
static unsigned char pillsPositions[MAX_PILLS][2];


// GAME INFO
#define MAX_LEVEL 10
const static unsigned char pillsPerLevel[MAX_LEVEL] = {
    2, 2, 3, 3, 4, 4, 5, 6, 7, 8
};
const static unsigned char speed[MAX_LEVEL] = {
    10, 10, 9, 9, 8, 8, 7, 6, 5, 4
};  
static unsigned char level = 0;
static unsigned char pad;
static unsigned char gameover;
static unsigned char i, j, k, l;
static unsigned char oamBuffer;
static unsigned char vFrameCount;

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
}


void drawSnake()
{
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
}


void growSnake()
{
    j = snakeSize;
    ++snakeSize;
    snakeSize = (snakeSize > MAX_SNAKE_SIZE - 1)? MAX_SNAKE_SIZE - 1 : snakeSize;
    if (j != snakeSize) {
        snakeCoords[snakeSize - 1][0] = snakeCoords[snakeSize - 2][0];
        snakeCoords[snakeSize - 1][1] = snakeCoords[snakeSize - 2][1];
    }
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

    reset();

    while(1)
    {
        ppu_wait_frame();
        gameover = 0;
        vFrameCount++;

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
        } else if (pad & PAD_A) {
            growSnake();
        }

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
        } else {
            for (i = 0; i < MAX_PILLS; i++) {  
                j = pillsPositions[i][0];
                k = pillsPositions[i][1];
                if (SnakeHead[0] == j && SnakeHead[1] == k) {
                    pillsPositions[i][0] = -1;
                    pillsPositions[i][1] = -1;
                    --pillsLive;
                    growSnake();
                }
            }
        }

        //Move snake
        if (vFrameCount > speed[level]) {
            vFrameCount = 0;
            
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
        // if (snakeSize > 3) {
        //     for (i = 3; i < snakeSize; i++) {
        //         gameover = gameover || (snakeCoords[i][0] == SnakeHead[0]) &&  (snakeCoords[i][1] == SnakeHead[1]);
        //         if (gameover) {
        //             break;
        //         }
        //     }
        // }

        oam_clear();
        oamBuffer = 0;

        drawSnake();

        //Draw pills
        for (i = 0; i < pillsPerLevel[level]; ++i) {
            if (pillsPositions[i][0] != -1 && pillsPositions[i][1] != -1) {
                oamBuffer = oam_spr(pillsPositions[i][0] * 8, pillsPositions[i][1] * 8, PILL_SPRITE, PILL_PALETTE, oamBuffer);
            }
        }

        if (gameover) {
            reset();
        }
    }
}