#include "neslib.h"

#include "level.h"

#define ATTRIBUTE_TABLE_A 0x23C0

#define SCREEN_WIDTH    256
#define SCREEN_HEIGHT   240

#define SNAKE_SPRITE_SIZE 8
#define SNAKE_SPRITE    0x42
#define SNAKE_PALETTE   0
#define PILL_SPRITE     0x40
#define PILL_PALETTE    1
#define WALL_SPRITE     0x10
#define WALL_PALETTE    1

#define MAX_SNAKE_SIZE  20
#define SNAKE_SPEED     2

#define SnakeHead  snakeCoords[0]

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

static unsigned char pad;
static unsigned char gameover;
static unsigned char i, x, y, dx, dy;

static unsigned char snakeCoords[MAX_SNAKE_SIZE][2];
static unsigned char snakeSize;

static unsigned char levelNamRef[1024];

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

    snakeSize = 1;
    snakeCoords[snakeSize - 1][0] = SCREEN_WIDTH >> 1;
    snakeCoords[snakeSize - 1][1] = SCREEN_HEIGHT >> 1;

    dy = SNAKE_SPEED;
    dx = 0;

    

    while(1)
    {
        ppu_wait_frame();

        for (i = 0; i < snakeSize; i++) {
            x = snakeCoords[i][0];
            y = snakeCoords[i][1];
            oam_spr(x, y, SNAKE_SPRITE, SNAKE_PALETTE, 0);
        }

        //Update snake body
        
        
        //Move head
        SnakeHead[0] += dx;
        SnakeHead[1] += dy;

        gameover = gameover || (SnakeHead[0] == (SCREEN_WIDTH - SNAKE_SPRITE_SIZE));
        gameover = gameover || (SnakeHead[1] == (SCREEN_HEIGHT - SNAKE_SPRITE_SIZE));

        if (gameover) {
            dx = dy = 0;
        }


        //Check game over
        //READ VRAM AND CHECK IF THERE IS A SPRITE THERE


        // ppu_wait_frame();//wait for next TV frame
        // oam_spr(x, y, POINTER_SPRITE, palette & 3, 0);

        // //Trigger for palette change
        // pad = pad_trigger(0);

        // //Converting if on & formula
        // // if (condition) {
        // //     x += t;
        // // }
        // // Can be expressed as:
        // // x += t & (0x0 - condition)
        // palette += 1 & (0x0 - ((pad & PAD_A) && 1));
        // palette -= 1 & (0x0 - ((pad & PAD_B) && 1));


        // //Poll for arrow movement
        // pad = pad_poll(0);
        // x -= POINTER_SPEED & (0x0 - (pad & PAD_LEFT && x >= 2));
        // x += POINTER_SPEED & (0x0 - (pad & PAD_RIGHT && x < (SCREEN_WIDTH - POINTER_SIZE)));
        // y -= POINTER_SPEED & (0x0 - (pad & PAD_UP && y >= 2));
        // y += POINTER_SPEED & (0x0 - (pad & PAD_DOWN && y < (SCREEN_HEIGHT - POINTER_SIZE)));
    }
}