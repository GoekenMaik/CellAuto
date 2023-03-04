#ifndef SHAREDRESOURCE_H
#define SHAREDRESOURCE_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct updateTexture
{
    SDL_Rect *textureWindow;
    SDL_Surface *texData;
} updateTexture;

typedef struct sharedResource
{
    bool update;
    bool keepOpen;
    SDL_Rect *textureWindow;
    sem_t semUpdate;
    sem_t semDraw;
    SDL_Surface *texData;
    SDL_Texture *texture;
    int scale;
    int zoom;
    int calculated_x;
    int calculated_y;
    int rule;
    int window_size_x;
    int window_size_y;
} sharedResource;

//SHAREDRESOURCE_H
#endif