#ifndef VIEW_H
#define VIEW_H

#include "sharedResource.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unistd.h>
#include <stdlib.h>

extern sharedResource shared;
SDL_Renderer *renderer;
SDL_Window *window;
SDL_Surface* iconLoading;
SDL_Texture *iconTexture;

void windowManager();
void init();
void draw();
void update();
void closeWindow();
extern void inputLoop();

//VIEW_H
#endif