#include "model.h"

bool rules[8];
int tocalc_x;
int tocalc_y;

void model()
// The main function of the model thread. Creates the rule array and the Automaton
{
    createRule(shared.rule);
    fillArray();
    // Notify the view that the texture is ready
    sem_post(&shared.semDraw);

    calcAutomatonLoop();
}

void fillArray()
// Creating the Automaton
{
    tocalc_x = shared.window_size_x * shared.scale;
    tocalc_y = shared.window_size_y * shared.scale / 2;
    shared.texData = SDL_CreateRGBSurface(0, tocalc_x, tocalc_y, 8, 0, 0, 0, 0);
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

int convTuple(SDL_Surface *texData, int x, int y)
// Converts a tuple to a number for indexing the rule array
{
    int result = 0;

    result += 1 * !(bool)(*((Uint8 *)texData->pixels + y * texData->pitch + x + 1));
    result += 2 * !(bool)(*((Uint8 *)texData->pixels + y * texData->pitch + x));
    result += 4 * !(bool)(*((Uint8 *)texData->pixels + y * texData->pitch + x - 1));

    return result;
}

int convTupleBegin(SDL_Surface *texData, int y)
// Converts the first tuple of a row. Fills up the leftmost cell with a white cell. This is necessary because the leftmost cell would be out of bounds otherwise
{
    int result = 0;

    result += 1 * !(bool)(*((Uint8 *)texData->pixels + (y * texData->pitch) + 1));
    result += 2 * !(bool)(*((Uint8 *)texData->pixels + (y * texData->pitch)));

    return result;
}

int convTupleEnd(SDL_Surface *texData, int y, int x)
// Converts the last tuple of a row. Fills up the rightmost cell with a white cell. This is necessary because the rightmost cell would be out of bounds otherwise
{
    int result = 0;

    result += 2 * !(bool)(*((Uint8 *)texData->pixels + (y * texData->pitch) + x - 1));
    result += 4 * !(bool)(*((Uint8 *)texData->pixels + (y * texData->pitch) + x));

    return result;
}

void createStart(SDL_Surface *texData, int x)
// Edit this function to change the starting row. The current starting row has a single black cell in the middle
{

    for (int i = 0; i < x; i++)
    {
        *((Uint8 *)texData->pixels + i) = WHITE;
    }
    *((Uint8 *)texData->pixels + (x / 2)) = BLACK;
}

void generateNext(SDL_Surface *texData, int y, int x)
// Generates the next row of the automaton
{
    if (rules[convTupleBegin(texData, y - 1)])
    {
        *((Uint8 *)texData->pixels + (y * texData->pitch)) = BLACK;
    }
    else
    {
        *((Uint8 *)texData->pixels + (y * texData->pitch)) = WHITE;
    }
    for (int i = 1; i < x - 1; i++)
    {
        if (rules[convTuple(texData, i, y - 1)])
        {
            *((Uint8 *)texData->pixels + (y * texData->pitch) + i) = BLACK;
        }
        else
        {
            *((Uint8 *)texData->pixels + (y * texData->pitch) + i) = WHITE;
        }
    }
    if (rules[convTupleEnd(texData, y - 1, x - 1)])
    {
        *((Uint8 *)texData->pixels + (y * texData->pitch) + x - 1) = BLACK;
    }
    else
    {
        *((Uint8 *)texData->pixels + (y * texData->pitch) + x - 1) = WHITE;
    }
}

void calcAutomatonLoop()
/* Observes the view and calculates the next model in the background when certain threshold of window on texture is exceeded */
{
    while (true)
    {
        {
            //printf("calculated_x: %d, textureWindow_x%d", shared.calculated_x, shared.textureWindow->x);
            if (
                ((float)shared.textureWindow->y + shared.window_size_y > shared.calculated_x / 5.25) ||
                (float)shared.textureWindow->x * 0.75 < (shared.calculated_x * 0.2 )||
                (float)(shared.textureWindow->x + shared.textureWindow->w) * 1.25 > (float)(shared.calculated_x * 0.8))
            {
                printf("Calculating new model\n");
                // if calculated_x * 2 > 32768, texture limit exceeded
                if (shared.calculated_x * 2 > 32768)
                {
                    // Maximum Size reached, close thread
                    return;
                }
                // Create new surface
                SDL_Surface *newSurface = SDL_CreateRGBSurface(0, shared.calculated_x * 2, shared.calculated_y * 2, 8, 0, 0, 0, 0);

                //Fill new surface with rule
                createStart(newSurface, shared.calculated_x * 2);
                for (int i = 1; i < shared.calculated_y * 2; i++)
                {
                    generateNext(newSurface, i, shared.calculated_x * 2);
                }

                // Update shared variables
                SDL_Surface *oldSurface = newSurface;
                // Wait for update in view to be finished and block access to shared variables
                sem_wait(&shared.semUpdate);
                shared.textureWindow->x = shared.textureWindow->x + shared.calculated_x / 2;
                shared.calculated_x *= 2;
                shared.calculated_y *= 2;
                shared.texData = newSurface;
                SDL_Palette *palette = SDL_AllocPalette(2);
                SDL_Color colors[2] = {{0, 0, 0, 255}, {255, 255, 255, 255}};
                SDL_SetPaletteColors(palette, colors, 0, 2);
                SDL_SetSurfacePalette(shared.texData, palette);

                // Signal update that new model is ready
                shared.update = true;
                sem_post(&shared.semUpdate);

                // Wait for update in view to be finished and free old surface
                while(shared.update){
                    usleep(1000);
                }
                SDL_FreeSurface(oldSurface);
                SDL_FreePalette(palette);
            }
            usleep(1000);
        }
    }
}