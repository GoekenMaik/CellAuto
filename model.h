#ifndef MODEL_H
#define MODEL_H

#include "sharedResource.h"
#include <unistd.h>

#define WHITE 1
#define BLACK 0

extern sharedResource shared;

void model();
void fillArray();
void createRule(int rule);
int convTuple(SDL_Surface* texData, int x, int y);
void createStart(SDL_Surface *texData, int x);
void generateNext(SDL_Surface *texData, int y, int x);
void calcAutomatonLoop();

//MODEL_H
#endif