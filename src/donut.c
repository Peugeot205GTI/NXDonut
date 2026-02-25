/*
**  NXDonut, spinning ascii 3d donut for the Nintendo Switch using libNX and MODPlay.
**  Donut.c by Andy Sloane, but actually readable.
**  Copyright Lololol.
*/

#include <stdio.h>
#include <string.h>
#include <switch.h>
#include <unistd.h>
#include "main.h"
#include <math.h>

void donut(PadState* pad)
{
    float z[1760];
    char b[1760];
    float c = 1;
    float d = 0;
    float e = 1;
    float a = 0;
    float g;
    float h;
    float G;
    float H;
    float t;
    float f;
    float D;
    float A2;
    int i;
    int j;
    int k;
    int x;
    int y;
    int o;
    int N;
    u64 kDown;

    while (appletMainLoop()){
        for (k = 0; k < 480; k++) {
            putchar(k % 80 ? ' ' : '\n');
        }
        padUpdate(pad);
        kDown = padGetButtonsDown(pad);
        if (kDown & HidNpadButton_Plus)
            break;
        memset(b, ' ', 1760);
        memset(z, 0, sizeof(z));
        g = 0;
        h = 1;
        for (j = 0; j < 90; j++) {
            G = 0;
            H = 1;
            for (i = 0; i < 314; i++) {
                A2 = h + 2;
                D = 1.0f / (G * A2 * a + g * e + 5.0f);
                t = G * A2 * e - g * a;
                x = 40 + (int)(30 * D * (H * A2 * d - t * c));
                y = 12 + (int)(15 * D * (H * A2 * c + t * d));
                o = x + 80 * y;
                N = 8 * ((g * a - G * h * e) * d
                    - G * h * a
                    - g * e
                    - H * h * c);
                if (y > 0 && y < 22 && x > 0 && x < 80 && D > z[o]) {
                    z[o] = D;
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
                }
                f = H;
                H -= 0.02f * G;
                G += 0.02f * f;
                f = (3 - H * H - G * G) / 2;
                H *= f;
                G *= f;
            }
            f = h;
            h -= 0.07f * g;
            g += 0.07f * f;
            f = (3 - h * h - g * g) / 2;
            h *= f;
            g *= f;
        }
        for (k = 0; k < 1760; k++) {
            putchar(k % 80 ? b[k] : '\n');
        }
        f = e;
        e -= 0.04f * a;
        a += 0.04f * f;
        f = (3 - e * e - a * a) / 2;
        e *= f;
        a *= f;
        f = d;
        d -= 0.02f * c;
        c += 0.02f * f;
        f = (3 - d * d - c * c) / 2;
        d *= f;
        c *= f;
        printf("\n\n\n\n\n\n");
        printf("\n \e[44;97m.----------------------------------------------------------------------------.\x1b[0m");
        printf("\n \e[44;97m|  NX Donut v1.0                                         (Press + to quit.)  |\x1b[0m");
        printf("\n \e[44;97m|  Based on the original donut.c by Andy Sloane <andy@a1k0n.net>             |\x1b[0m");
		printf("\n \e[44;97m|  Ported by ToyotaAE86Trueno <toyota_ae86trueno> (Discord)                  |\x1b[0m");
		printf("\n \e[44;97m|  Music by Jogeir Liljedahl                                                 |\x1b[0m");
		printf("\n \e[44;97m'----------------------------------------------------------------------------'\x1b[0m");
        printf("\x1b[40A");
        consoleUpdate(NULL);
    }
}