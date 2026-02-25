/*
**  NXDonut.
**  Main file.
**  Copyright Lololol.
*/

#include "../MODPlay/modplay.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    consoleInit(NULL);
    nwindowSetSwapInterval(nwindowGetDefault(), 2);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    donut(&pad);
    consoleExit(NULL);
    return 0;
}