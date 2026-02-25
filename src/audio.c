#include "audio.h"

#include "../MODPlay/modplay.h"

#if __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#elif __has_include(<SDL.h>)
#include <SDL.h>
#else
#error "SDL headers not found (expected SDL2/SDL.h or SDL.h)"
#endif
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

typedef struct {
    Thread thread;
    uint8_t threadStack[64 * 1024] __attribute__((aligned(0x1000)));
    volatile bool running;
    SDL_AudioDeviceID device;
    SDL_AudioSpec spec;
    int16_t *mixBuffer;
    uint32_t mixFrames;
    uint8_t *modData;
    size_t modSize;
} AudioRuntime;

static AudioRuntime gAudio;

static bool loadModFile(const char *path, uint8_t **outData, size_t *outSize)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        return false;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return false;
    }

    long size = ftell(f);
    if (size <= 0) {
        fclose(f);
        return false;
    }

    rewind(f);

    uint8_t *buf = (uint8_t *)malloc((size_t)size);
    if (!buf) {
        fclose(f);
        return false;
    }

    size_t readCount = fread(buf, 1, (size_t)size, f);
    fclose(f);

    if (readCount != (size_t)size) {
        free(buf);
        return false;
    }

    *outData = buf;
    *outSize = (size_t)size;
    return true;
}

static void audioThreadMain(void *arg)
{
    AudioRuntime *audio = (AudioRuntime *)arg;
    const uint32_t bytesPerChunk = audio->mixFrames * sizeof(int16_t) * 2;
    const uint32_t minQueuedBytes = bytesPerChunk * 4;

    while (audio->running) {
        uint32_t queued = SDL_GetQueuedAudioSize(audio->device);

        if (queued < minQueuedBytes) {
            RenderMOD(audio->mixBuffer, (int)audio->mixFrames);
            SDL_QueueAudio(audio->device, audio->mixBuffer, bytesPerChunk);
        } else {
            svcSleepThread(2 * 1000 * 1000);
        }
    }
}

int audioStart(const char *modPath)
{
    if (gAudio.running) {
        return 0;
    }

    memset(&gAudio, 0, sizeof(gAudio));

    if (!loadModFile(modPath, &gAudio.modData, &gAudio.modSize)) {
        printf("Failed to open module: %s\n", modPath);
        return -1;
    }

    const int sampleRate = 48000;
    if (!InitMOD(gAudio.modData, sampleRate)) {
        printf("Invalid module format: %s\n", modPath);
        free(gAudio.modData);
        memset(&gAudio, 0, sizeof(gAudio));
        return -1;
    }

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        printf("SDL audio init failed: %s\n", SDL_GetError());
        free(gAudio.modData);
        memset(&gAudio, 0, sizeof(gAudio));
        return -1;
    }

    SDL_AudioSpec wanted;
    SDL_zero(wanted);
    wanted.freq = sampleRate;
    wanted.format = AUDIO_S16SYS;
    wanted.channels = 2;
    wanted.samples = 1024;
    wanted.callback = NULL;

    gAudio.device = SDL_OpenAudioDevice(NULL, 0, &wanted, &gAudio.spec, 0);
    if (!gAudio.device) {
        printf("SDL open audio failed: %s\n", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        free(gAudio.modData);
        memset(&gAudio, 0, sizeof(gAudio));
        return -1;
    }

    gAudio.mixFrames = gAudio.spec.samples;
    gAudio.mixBuffer = (int16_t *)malloc(gAudio.mixFrames * sizeof(int16_t) * 2);
    if (!gAudio.mixBuffer) {
        SDL_CloseAudioDevice(gAudio.device);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        free(gAudio.modData);
        memset(&gAudio, 0, sizeof(gAudio));
        return -1;
    }

    gAudio.running = true;

    Result rc = threadCreate(
        &gAudio.thread,
        audioThreadMain,
        &gAudio,
        gAudio.threadStack,
        sizeof(gAudio.threadStack),
        0x2B,
        1
    );

    if (R_FAILED(rc)) {
        printf("Audio thread create failed: 0x%08X\n", rc);
        gAudio.running = false;
        free(gAudio.mixBuffer);
        SDL_CloseAudioDevice(gAudio.device);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        free(gAudio.modData);
        memset(&gAudio, 0, sizeof(gAudio));
        return -1;
    }

    rc = threadStart(&gAudio.thread);
    if (R_FAILED(rc)) {
        printf("Audio thread start failed: 0x%08X\n", rc);
        gAudio.running = false;
        threadClose(&gAudio.thread);
        free(gAudio.mixBuffer);
        SDL_CloseAudioDevice(gAudio.device);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        free(gAudio.modData);
        memset(&gAudio, 0, sizeof(gAudio));
        return -1;
    }

    SDL_PauseAudioDevice(gAudio.device, 0);
    return 0;
}

void audioStop(void)
{
    if (!gAudio.device) {
        return;
    }

    gAudio.running = false;
    threadWaitForExit(&gAudio.thread);
    threadClose(&gAudio.thread);

    SDL_ClearQueuedAudio(gAudio.device);
    SDL_CloseAudioDevice(gAudio.device);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    free(gAudio.mixBuffer);
    free(gAudio.modData);
    memset(&gAudio, 0, sizeof(gAudio));
}
