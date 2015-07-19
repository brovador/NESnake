//Move a pointer around all the screen
//Change palette when pressing A or B
#include "neslib.h"

#define SCREEN_WIDTH    256
#define SCREEN_HEIGHT   240

#define POINTER_SPRITE  0x41
#define POINTER_SIZE    8
#define POINTER_SPEED   2

const unsigned char spritesPal[16] = {
    0x0F, 0x11, 0x21, 0x31,
    0x0F, 0x12, 0x22, 0x32,
    0x0F, 0x13, 0x23, 0x33,
    0x0F, 0x14, 0x24, 0x34
};

static unsigned char x,y,pad,palette;

void main(void)
{
    pal_spr(spritesPal);
    ppu_on_all();

    palette = x = y = 0;

    while(1)
    {
        ppu_wait_frame();//wait for next TV frame
        oam_spr(x, y, POINTER_SPRITE, palette & 3, 0);

        //Trigger for palette change
        pad = pad_trigger(0);

        //Converting if on & formula
        // if (condition) {
        //     x += t;
        // }
        // Can be expressed as:
        // x += t & (0x0 - condition)
        palette += 1 & (0x0 - ((pad & PAD_A) && 1));
        palette -= 1 & (0x0 - ((pad & PAD_B) && 1));

        //Poll for arrow movement
        pad = pad_poll(0);
        
        
        x -= POINTER_SPEED & (0x0 - (pad & PAD_LEFT && x >= 2));
        x += POINTER_SPEED & (0x0 - (pad & PAD_RIGHT && x < (SCREEN_WIDTH - POINTER_SIZE)));
        y -= POINTER_SPEED & (0x0 - (pad & PAD_UP && y >= 2));
        y += POINTER_SPEED & (0x0 - (pad & PAD_DOWN && y < (SCREEN_HEIGHT - POINTER_SIZE)));
    }
}