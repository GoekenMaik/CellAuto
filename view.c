#include "view.h"

void windowManager()
// Window manager with main Loop
{
    init();
    // Wait for the model to create Surface with Rule
    sem_wait(&shared.semDraw);
    // Draw the texture to the window
    draw();
    // Signal the model that the texture is drawn
    sem_post(&shared.semDraw);
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
    // Create Title
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
    iconLoading = IMG_Load("loading.bmp");
    iconTexture = SDL_CreateTextureFromSurface(renderer, iconLoading);
}

void draw()
// Draws the texture to the window
{
    /* Create a palette to define colors for values BLACK/0 and WHITE/1
    The palette is then applied to the surface and the surface is used to create the texture */
    SDL_Palette *palette = SDL_AllocPalette(2);
    SDL_Color colors[2] = {{0, 0, 0, 255}, {255, 255, 255, 255}};
    SDL_SetPaletteColors(palette, colors, 0, 2);
    SDL_SetSurfacePalette(shared.texData, palette);
    shared.texture = SDL_CreateTextureFromSurface(renderer, shared.texData);

    // Draw the texture to the window
    SDL_RenderCopy(renderer, shared.texture, shared.textureWindow, NULL);
    SDL_RenderPresent(renderer);
    SDL_FreePalette(palette);
    SDL_FreeSurface(shared.texData);
}

void closeWindow()
// Closes the window and frees the memory
{
    SDL_DestroyTexture(shared.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void update()
// Updates the texture and draws it to the window
{
    // Block access to shared resource
    sem_wait(&shared.semUpdate);
    // If the model has created a new surface, the texture will be updated
    if (shared.update)
    {
        // Draw the loading icon
        SDL_Rect iconRect = (SDL_Rect){50, shared.window_size_y - 100, 100, 36};
        SDL_RenderCopy(renderer, iconTexture, NULL, &iconRect);
        SDL_RenderPresent(renderer);

        // Update the texture
        SDL_DestroyTexture(shared.texture);
        shared.texture = SDL_CreateTextureFromSurface(renderer, shared.texData);
        if (shared.texture == NULL)
        {
            printf("Error: %s\n", SDL_GetError());
            exit(1);
        }
        // inform the model that the texture has been updated
        shared.update = false;
    }

    // Update the screen
    sem_post(&shared.semUpdate);
    SDL_RenderCopy(renderer, shared.texture, shared.textureWindow, NULL);
    SDL_RenderPresent(renderer);
}