#include <conio.h>
#include "neslib.h"
#include "level.h"

//LEVEL INFO
#define SCREEN_WIDTH    256
#define SCREEN_HEIGHT   240

#define ATTRIBUTETABLE_A 0x23C0

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

static unsigned char levelNamRef[1024];

static unsigned char pad;
static unsigned char palette;

static unsigned char sprite = 0x40;
static unsigned char oamBuffer;

#define VRAM_BUFFER_SIZE 3 * 3 + 1

const static unsigned char vramBuffer[VRAM_BUFFER_SIZE ] = {
    MSB(NTADR_A(1,1)),LSB(NTADR_A(1,1)),0x40,
    MSB(NTADR_A(1,2)),LSB(NTADR_A(1,2)),0x40,
    MSB(NTADR_A(1,3)),LSB(NTADR_A(1,3)),0x40,
    NT_UPD_EOF
};
static unsigned char vramBufferCopy[VRAM_BUFFER_SIZE];

void main(void)
{
    //PALETTE SETUP
    pal_spr(spritesPal);
    pal_bg(bgPal);

    memcpy(vramBufferCopy, vramBuffer, sizeof(vramBuffer));
    set_vram_update(vramBufferCopy);

    //LEVEL LOAD
    //TODO: remove copy by packing level_nam contents
    // memcpy(levelNamRef, level_nam, 1024);
    // vram_adr(NAMETABLE_A);
    // vram_write(levelNamRef, 1024);

    ppu_on_all();

    while(1)
    {
        ppu_wait_frame();

        //Pad input
        pad = pad_trigger(0);
        if (pad & PAD_A) {
            ++palette;
        }

        //Sprites draw
        oam_clear();
        oamBuffer = 0;
        oamBuffer = oam_spr(0, 0, sprite, palette & 3, oamBuffer);
        oamBuffer = oam_spr(6, 6, sprite, palette & 3, oamBuffer);
        oamBuffer = oam_spr(0, 16, sprite, palette & 3, oamBuffer);
    }
}