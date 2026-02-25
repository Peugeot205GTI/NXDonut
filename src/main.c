/*
**  NXDonut, spinning ascii 3d donut for the Nintendo Switch using libNX and MODPlay.
**  Main file.
**  Copyright Lololol.
*/

#include "../MODPlay/modplay.h"
#include "audio.h"
#include "main.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    consoleInit(NULL);
    nwindowSetSwapInterval(nwindowGetDefault(), 2);

    bool romfsMounted = false;
    Result rc = romfsInit();
    if (R_SUCCEEDED(rc)) {
        romfsMounted = true;
    } else {
        printf("romfs init failed: 0x%08X\n", rc);
    }

    const char *modPath = (argc > 1) ? argv[1] : "romfs:/music.mod";
    if (audioStart(modPath) != 0) {
        printf("Audio disabled (module path: %s)\n", modPath);
    }

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    donut(&pad);

    audioStop();
    if (romfsMounted) {
        romfsExit();
    }

    consoleExit(NULL);
    return 0;
}