#include "ssiv.h"
#include <SDL3/SDL_stdinc.h>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_AppResult retval = SDL_APP_CONTINUE;
  SDL_Surface *text;

  parse_options(argc, argv);

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    printf("Failed: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("Ssiv", 800, 600,
                                   SDL_WINDOW_HIGH_PIXEL_DENSITY |
                                       SDL_WINDOW_BORDERLESS |
                                       SDL_WINDOW_TRANSPARENT,
                                   &window, &renderer)) {
    printf("Failed: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetRenderVSync(renderer, 1);

  // SDL_SetRenderLogicalPresentation(renderer, original_width, original_height,
  //                                  SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
  SDL_SetAppMetadata("Simple SDL Image Viewer", "0.1", "com.everyway.ssiv");

  if (!TTF_Init()) {
    SDL_Log("Couldn't initialize SDL_ttf: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  char *font_path = "/usr/share/fonts/ibmplexmono/IBMPlexMono-Regular.ttf";
  font = TTF_OpenFont(font_path, 14.0f);

  if (!font) {
    SDL_Log("Couldn't Open Font: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  text = TTF_RenderText_Blended(font, argv[1], 0,
                                text_color); // Why base path here?

  if (!text) {
    SDL_Log("Couldn't create text: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  text_texture = SDL_CreateTextureFromSurface(renderer, text);
  SDL_DestroySurface(text);

  if (!text_texture) {
    SDL_Log("Couldn't create text texture: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  retval = find_files(argc, argv);

  if (retval != SDL_APP_CONTINUE)
    return retval;

  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  Draw_StatusBar(appstate);
  SDL_RenderPresent(renderer);

  num_photos = 0;
  for (int h = 0; h < pathcount; h++) {
    num_photos += photos[h].filenames_size;
  }

  thumbnail_Textures = malloc(num_photos * sizeof(SDL_Texture *));
  if (!thumbnail_Textures) {
    printf("Texture re-allocation failed!");
    return SDL_APP_FAILURE;
  }
  thumbnail_Surfaces = malloc(num_photos * sizeof(SDL_Surface *));
  if (!thumbnail_Surfaces) {
    printf("Texture re-allocation failed!");
    return SDL_APP_FAILURE;
  }
  full_Textures = malloc(num_photos * sizeof(SDL_Texture *));
  if (!full_Textures) {
    printf("Texture re-allocation failed!");
    return SDL_APP_FAILURE;
  }
  full_Surfaces = malloc(num_photos * sizeof(SDL_Surface *));
  if (!full_Surfaces) {
    printf("Texture re-allocation failed!");
    return SDL_APP_FAILURE;
  }

  for (int i = 0; i < num_photos; i++) {
    thumbnail_Textures[i] = NULL;
    thumbnail_Surfaces[i] = NULL;
    full_Textures[i] = NULL;
    full_Surfaces[i] = NULL;
  }

  // Put this in its own thread
  pthread_t render_thread;
  pthread_create(&render_thread, NULL, render_threads, NULL);

  need_redraw = true;

  return retval;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  SDL_AppResult retval;
  switch (g_state) {
  case THUMBNAILER:
    retval = thumbnailer_AppEvent(appstate, event);
    break;
  case IMAGE:
    retval = thumbnailer_AppEvent(appstate, event);
    break;
  }
  return retval;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  SDL_AppResult retval;
  switch (g_state) {
  case THUMBNAILER:
    retval = Draw_Thumbnails(appstate);
    break;
  case IMAGE:
    retval = Draw_Image(appstate);
    break;
  }

  return retval;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  for (int i = 0; i < num_photos; ++i) {
    SDL_DestroyTexture(thumbnail_Textures[i]);
    SDL_DestroyTexture(full_Textures[i]);
  }

  for (int i = 0; i < pathcount; i++) {
    for (int j = 0; j < photos[i].filenames_size; j++) {
      free(photos[i].filenames[j]);
    }
  }
  free(photos);

  // Closing fonts
  if (font) {
    TTF_CloseFont(font);
  }
  TTF_Quit();

  // Destroy the window created and renderer
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
}
