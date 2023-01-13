#include "model.h"

bool rules[8];
int tocalc_x;
int tocalc_y;

void model()
// The main function of the model thread. Creates the rule array and the Automaton
{
    createRule(shared.rule);
    pthread_mutex_lock(&shared.lock);
    fillArray();
    shared.update = true;
    //calcAutomatonLoop();
}

void fillArray()
// Creating the Automaton
{
    tocalc_x = shared.window_size_x * shared.scale;
    tocalc_y = shared.window_size_y * shared.scale;
    shared.texData = SDL_CreateRGBSurface(0, tocalc_x, tocalc_y, 32, 0, 0, 0, 0);
    createStart(shared.texData, tocalc_x);

    for (int i = 1; i < tocalc_y; i++)
    {
        generateNext(shared.texData, i, tocalc_x);
    }

    shared.calculated_x = tocalc_x;
    shared.calculated_y = tocalc_y;
}

void createRule(int rule)
// Creates the rule array from the rule number
{
    for (int i = 0; i < 8; i++)
    {
        rules[i] = (rule >> i) & 1;
    }
}

int convTuple(int x, int y)
// Converts a tuple to a number for indexing the rule array
{
    int result = 0;

    result += 1 * !(bool)(*((Uint32 *)shared.texData->pixels + y * shared.texData->pitch / 4 + x + 1));
    result += 2 * !(bool)(*((Uint32 *)shared.texData->pixels + y * shared.texData->pitch / 4 + x));
    result += 4 * !(bool)(*((Uint32 *)shared.texData->pixels + y * shared.texData->pitch / 4 + x - 1));

    return result;
}

int convTupleBegin(int y)
// Converts the first tuple of a row. Fills up the leftmost cell with a white cell. This is necessary because the leftmost cell would be out of bounds otherwise
{
    int result = 0;

    result += 1 * !(bool)(*((Uint32 *)shared.texData->pixels + (y * shared.texData->pitch / 4) + 1));
    result += 2 * !(bool)(*((Uint32 *)shared.texData->pixels + (y * shared.texData->pitch / 4)));

    return result;
}

int convTupleEnd(int y)
// Converts the last tuple of a row. Fills up the rightmost cell with a white cell. This is necessary because the rightmost cell would be out of bounds otherwise
{
    int result = 0;

    result += 2 * !(bool)(*((Uint32 *)shared.texData->pixels + (y * shared.texData->pitch / 4) + tocalc_x - 1));
    result += 4 * !(bool)(*((Uint32 *)shared.texData->pixels + (y * shared.texData->pitch / 4) + tocalc_x));

    return result;
}

void createStart(SDL_Surface *texData, int x)
// Edit this function to change the starting row. The current starting row has a single black cell in the middle
{

    for (int i = 0; i < x; i++)
    {
        *((Uint32 *)texData->pixels + i) = WHITE;
    }
    *((Uint32 *)texData->pixels + (tocalc_x / 2)) = BLACK;
}

void generateNext(SDL_Surface *texData, int y, int x)
// Generates the next row of the automaton
{
    if (rules[convTupleBegin(y - 1)])
    {
        *((Uint32 *)texData->pixels + (y * texData->pitch / 4)) = BLACK;
    }
    else
    {
        *((Uint32 *)texData->pixels + (y * texData->pitch / 4)) = WHITE;
    }
    for (int i = 1; i < x - 1; i++)
    {
        if (rules[convTuple(i, y - 1)])
        {
            *((Uint32 *)texData->pixels + (y * texData->pitch / 4) + i) = BLACK;
        }
        else
        {
            *((Uint32 *)texData->pixels + (y * texData->pitch / 4) + i) = WHITE;
        }
    }
    if (rules[convTupleEnd(y - 1)])
    {
        *((Uint32 *)texData->pixels + (y * texData->pitch / 4) + x - 1) = BLACK;
    }
    else
    {
        *((Uint32 *)texData->pixels + (y * texData->pitch / 4) + x - 1) = WHITE;
    }
}

//void calcAutomatonLoop()
//{
//    while (true)
//    {
//        {
//            if (((float)(shared.textureWindow->y + shared.textureWindow->h) / (float)shared.calculated_y) > 0.8 || ((float)(shared.textureWindow->x + shared.textureWindow->w) / (float)shared.calculated_x) > 0.8 || (float)(shared.textureWindow->x / (float)shared.calculated_x) < 0.2)
//            {
//                printf("CALCULATING NEW MODEL\n");
//                SDL_Surface *texData = SDL_CreateRGBSurface(0, shared.calculated_x * 2, shared.calculated_y * 2, 32, 0, 0, 0, 0);
//                printf("rows: %i\n", shared.calculated_y * 2);
//                createStart(texData, shared.calculated_x * 2);
//                for (int i = 1; i < shared.calculated_y * 2; i++)
//                {
//                    printf("row %i\n", i);
//                    //generateNext(texData, i, shared.calculated_x);
//                }
//                shared.calculated_x *= 2;
//                shared.calculated_y *= 2;
//                printf("updated\n");
//            }
//            usleep(1000);
//        }
//    }
//}