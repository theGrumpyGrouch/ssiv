#include "gsiv.h"
#include <pthread.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

// SDL I/O
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Event *event = NULL;
TTF_Font *font = NULL;
int original_height = 600;
int original_width = 800;

// File I/O
char *base_path = NULL;
char **filenames = NULL;
char *filename = NULL;
char last_path[4096];
int filenames_size = 0;
int filenames_capacity = 0;
SDL_Color text_color = {0,0,0, SDL_ALPHA_OPAQUE};
SDL_Texture *text_texture = NULL;

// MISC
bool need_redraw = true;
bool textures_generated = false;
int cursor_pos = 0;
int offset_pos = 0;
AppMode g_state = IMAGE;

//Mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// Textures
SDL_Texture **thumbnail_Textures = NULL;
SDL_Surface **thumbnail_Surfaces = NULL;
SDL_Texture **full_Textures = NULL;
SDL_Surface **full_Surfaces = NULL;
