#include "view.h"

void windowManager()
// Window manager with main Loop
{
    init();
    pthread_mutex_unlock(&shared.lock);
    while (!shared.update)
        usleep(100);
    draw();
    shared.update = !shared.update;
    // Main Loop
    while (shared.keepOpen)
    {
        inputLoop();
        update();
    }
    closeWindow();
}

void init()
/* Initialize the SDL library and creates a window. Uses VSync.
The texture will be manipulated such that it gets moved pixel by pixel
default values are setup by the controller
no filter is used */
{
    //Create Title
    char windowTitle[] = "Rule ";
    char ruleNumber[10];
    sprintf(ruleNumber, "%d", shared.rule);

    // Initialize the SDL library
    SDL_Init(SDL_INIT_VIDEO);

    // Create a window
    window = SDL_CreateWindow(strcat(windowTitle, ruleNumber),
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              shared.window_size_x, shared.window_size_y,
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    // Create a renderer for the window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void draw()
// Draws the texture to the window
{
    texture = SDL_CreateTextureFromSurface(renderer, shared.texData);
    SDL_RenderCopy(renderer, texture, shared.textureWindow, NULL);
    SDL_RenderPresent(renderer);
}

void closeWindow()
// Closes the window and frees the memory
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void update()
// Updates the texture and draws it to the window
{
    SDL_RenderCopy(renderer, texture, shared.textureWindow, NULL);
    SDL_RenderPresent(renderer);
}