#include "controller.h"

int main(int argc, char *argv[])
{
    // Creating the model thread
    pthread_t modelThread;
    shared.rule = 30;
    // Checking for rule input
    if (argc != 2)
    {
        // if no rule is given, ask for one
        printf("Input Rule: ");
        scanf("%d", &shared.rule);
    }
    else
    {
        // Using rule from command line
        shared.rule = atoi(argv[1]);
    }

    if (shared.rule < 0 || shared.rule > 255)
    {
        // Checking if rule is between 0 and 255
        printf("Rule must be between 0 and 255");
        return 0;
    }

    // Starting the program, initializing shared resource and creating the model thread
    startup();
    pthread_create(&modelThread, NULL, (void *)model, NULL);
    windowManager();
}

void startup()
{
    //  Setting up shared resource with default values
    shared.window_size_x = RESOLUTION_X;
    shared.window_size_y = RESOLUTION_Y;
    shared.scale = SCALE;
    shared.zoom = 1;
    shared.keepOpen = true;
    sem_init(&shared.semUpdate, 0, 1);
    sem_init(&shared.semDraw, 0, 0);
    shared.textureWindow = malloc(sizeof(SDL_Rect));
    shared.textureWindow->x = (shared.scale * shared.window_size_x) / 2 - ((shared.window_size_x) / shared.zoom / 2);
    shared.textureWindow->y = 0;
    shared.textureWindow->w = shared.window_size_x / shared.zoom;
    shared.textureWindow->h = shared.window_size_y / shared.zoom;
}

void inputLoop()
// Input Loop. Changes bool shared.update to notify the view to update. Gets called in the windowManager
{
    while (SDL_PollEvent(&event))
    {
        // moves the texture window when the mouse is dragged
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
        {
            left_button_down = true;
            mouse_x = event.button.x;
            mouse_y = event.button.y;
            tmp_x = shared.textureWindow->x;
            tmp_y = shared.textureWindow->y;
            continue;
        }
        else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
        {
            left_button_down = false;
            continue;
        }
        else if (event.type == SDL_MOUSEMOTION && left_button_down)
        {
            if (shared.update)
            {
                tmp_x = shared.textureWindow->x;
                tmp_y = shared.textureWindow->y;
                //left_button_down = false;
                continue;
            }
            tmp_x -= (event.motion.x - mouse_x) / shared.zoom;
            tmp_y -= (event.motion.y - mouse_y) / shared.zoom;
            shared.textureWindow->x = tmp_x;
            shared.textureWindow->y = tmp_y;
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
            if (shared.textureWindow->y < 0)
                shared.textureWindow->y = 0;

            checkWindowPos();
            continue;
        }

        // zooms in and out when the mouse wheel is used
        if (event.type == SDL_MOUSEWHEEL)
        {
            if (event.wheel.y == -1)
            {
                zoomOut();
                continue;
            }
            if (event.wheel.y == 1)
            {
                zoomIn();
                continue;
            }
        }

        // moves the texture window when array keys or wasd are pressed
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_UP:
                shared.textureWindow->y -= 1;
                if (shared.textureWindow->y < 0)
                    shared.textureWindow->y = 0;
                break;
            case SDLK_DOWN:
                shared.textureWindow->y += 1;
                break;
            case SDLK_LEFT:
                shared.textureWindow->x -= 1;
                break;
            case SDLK_RIGHT:
                shared.textureWindow->x += 1;
                break;
            case SDLK_w:
                shared.textureWindow->y -= 10;
                if (shared.textureWindow->y < 0)
                    shared.textureWindow->y = 0;
                break;
            case SDLK_s:
                shared.textureWindow->y += 10;
                break;
            case SDLK_a:
                shared.textureWindow->x -= 10;
                break;
            case SDLK_d:
                shared.textureWindow->x += 10;
                break;
            case SDLK_KP_PLUS:
                zoomIn();
                break;
            case SDLK_KP_MINUS:
                zoomOut();
                break;
            }
            checkWindowPos();
            continue;
        }
        // If the user resizes the window
        if (event.type == SDL_WINDOWEVENT)
        {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                resizeWindow(event.window.data1, event.window.data2);
                continue;
            }
        }

        // If the user closes the window
        if (event.type == SDL_QUIT)
        {
            shared.keepOpen = false;
        }
    }
}

void zoomIn()
// Zooms in on the texture window
{
    if (shared.zoom == 256)
    {
        return;
    }
    shared.zoom *= 2;
    shared.textureWindow->w = shared.window_size_x / shared.zoom;
    shared.textureWindow->h = shared.window_size_y / shared.zoom;
    shared.textureWindow->x = shared.textureWindow->x + shared.textureWindow->w / 2;
    shared.textureWindow->y = shared.textureWindow->y + shared.textureWindow->h / 2;
    checkWindowPos();
}

void zoomOut()
// Zooms out on the texture window
{
    if (shared.zoom == 1)
    {
        return;
    }
    shared.zoom /= 2;
    shared.textureWindow->y = shared.textureWindow->y - shared.textureWindow->h / 2;
    if (shared.textureWindow->y < 0)
        shared.textureWindow->y = 0;
    shared.textureWindow->x = shared.textureWindow->x - shared.textureWindow->w / 2;
    shared.textureWindow->w = shared.window_size_x / shared.zoom;
    shared.textureWindow->h = shared.window_size_y / shared.zoom;
    checkWindowPos();
}

void resizeWindow(int newWidth, int newHeight)
{
    // resize the window
    shared.window_size_x = newWidth;
    shared.window_size_y = newHeight;
    shared.textureWindow->w = newWidth / shared.zoom;
    shared.textureWindow->h = newHeight / shared.zoom;
    checkWindowPos();
}

void checkWindowPos()
{
    // check if the window is out of bounds, if so, move it back in
    if (shared.textureWindow->x < shared.calculated_x / 4)
        shared.textureWindow->x = shared.calculated_x / 4;
    if (shared.textureWindow->y < 0)
        shared.textureWindow->y = 0;
    if (shared.textureWindow->x + shared.textureWindow->w > shared.calculated_x - shared.calculated_x / 4)
        shared.textureWindow->x = shared.calculated_x - shared.textureWindow->w - shared.calculated_x / 4;
    if (shared.textureWindow->y + shared.textureWindow->h > shared.calculated_x / 4)
        shared.textureWindow->y = (shared.calculated_x / 4) - shared.textureWindow->h;
}