#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include "sharedResource.h"

#define RESOLUTION_X 800
#define RESOLUTION_Y 600
#define SCALE 8

sharedResource shared;
SDL_Event event;
float mouse_x = 0;
float mouse_y = 0;
float tmp_x = 0;
float tmp_y = 0;
bool left_button_down = false;

void startup();
void inputLoop();
void zoomIn();
void zoomOut();
void checkWindowPos();
void resizeWindow(int newWidth, int newHeight);
extern void model();
extern void windowManager();
extern void calcAutomaton();

//CONTROLLER_H
#endif