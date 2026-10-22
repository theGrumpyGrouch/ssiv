#include "ssiv.h"
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
photo_album_t *photos = NULL;
char** paths = NULL;
int pathcount = 0;
int num_photos = 0;

char last_path[4096];
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

// Backend
ActionBackend feh_backend = {
  .name = "feh",
  .init = feh_init,
  .do_action = feh_set_wallpaper,
  .cleanup = feh_cleanup
};

ActionBackend arg_backend = {
  .name = "",
  .init = NULL,
  .do_action = arg_action,
  .cleanup = feh_cleanup
};
