#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <assert.h>
#include <dirent.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Custom Type Definitions
typedef enum { THUMBNAILER, IMAGE } AppMode;
typedef struct {
  SDL_Texture **appstate;
  int i;
  int j;
} texture_context;

// Variables

// SDL I/O
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Event *event;
extern TTF_Font *font;
extern int original_height;
extern int original_width;

// Textures
extern SDL_Texture **thumbnail_Textures;
extern SDL_Surface **thumbnail_Surfaces;
extern SDL_Texture **full_Textures;
extern SDL_Surface **full_Surfaces;

// File I/O
extern char *base_path;
extern char last_path[4096];
extern char **filenames;
extern int filenames_size;
extern int filenames_capacity;
extern char *filename;
extern SDL_Color text_color;
extern SDL_Texture *text_texture;

// MISC
extern bool need_redraw;
extern bool textures_generated;
extern int cursor_pos;
extern int offset_pos;
extern AppMode g_state;
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;

// Functions

// SDL
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]);
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event);
SDL_AppResult SDL_AppIterate(void *appstate);
void SDL_AppQuit(void *appstate, SDL_AppResult result);

// Custom

// Render
SDL_AppResult thumbnailer_AppEvent(void **appstate, SDL_Event *event);
SDL_AppResult Image_AppEvent(void **appstate, SDL_Event *event);
SDL_AppResult Draw_StatusBar(void *appstate);
SDL_AppResult Draw_Thumbnails(void *appstate);
SDL_AppResult Draw_Image(void *appstate);
void draw_roundedRect(int x, int y, int w, int h, int r);

// File handling
SDL_AppResult find_files();
void build_image_textures(void **appstate);
void *load_thumbnail_Surface(void *args);
void *render_threads(void *args);

// Algorithms
SDL_Surface *scale_surface_bilinear(SDL_Surface *src, int width, int height, int img_pos);
SDL_Surface *scale_surface_stb(SDL_Surface *src, int width, int height,
                               int img_pos);
