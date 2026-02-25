/*
**  NXDonut
**  Main header file.
**  Copyright Lololol.
*/

#ifndef NXD_MAIN_H
    #define NXD_MAIN_H
    #define TARGET_FRAME_TIME (1000/30)
    #include <switch.h>
void donut(PadState* pad);
int audioStart(const char *modPath);
void audioStop(void);
#endif