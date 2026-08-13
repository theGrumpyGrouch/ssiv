#include "gsiv.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>

// Event
SDL_AppResult Image_AppEvent(void **appstate, SDL_Event *event) {
  SDL_Keycode key;
  int old_pos = cursor_pos;
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
    break;
  case SDL_EVENT_KEY_DOWN:
    need_redraw = true;
    key = event->key.key;

    if (key == SDLK_L)
      cursor_pos += 1;
    else if (key == SDLK_H)
      cursor_pos -= 1;
    else if (key == SDLK_J)
      cursor_pos += 5;
    else if (key == SDLK_K)
      cursor_pos -= 5;
    else if (key == SDLK_RETURN)
      g_state = IMAGE;
    else if (key == SDLK_ESCAPE)
      g_state = THUMBNAILER;
    else if (key == SDLK_Q)
      return SDL_APP_SUCCESS;

    if (cursor_pos >= filenames_size)
      cursor_pos = filenames_size - 1;
    else if (cursor_pos < 0)
      cursor_pos = 0;

    if ((abs(old_pos - cursor_pos) != 5 &&
         ((key == SDLK_J) || (key == SDLK_K))))
      cursor_pos = old_pos;
    break;
  }

  return SDL_APP_CONTINUE;
}
SDL_AppResult thumbnailer_AppEvent(void **appstate, SDL_Event *event) {
  SDL_Keycode key;
  int old_pos = cursor_pos;
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
    break;
  case SDL_EVENT_KEY_DOWN:
    need_redraw = true;
    key = event->key.key;

    if (key == SDLK_L)
      cursor_pos += 1;
    else if (key == SDLK_H)
      cursor_pos -= 1;
    else if (key == SDLK_J)
      cursor_pos += 5;
    else if (key == SDLK_K)
      cursor_pos -= 5;
    else if (key == SDLK_R) {
      need_redraw = true;
    } else if (key == SDLK_RETURN)
      g_state = IMAGE;
    else if (key == SDLK_ESCAPE)
      g_state = THUMBNAILER;
    else if (key == SDLK_Q)
      return SDL_APP_SUCCESS;

    if (cursor_pos >= filenames_size)
      cursor_pos = filenames_size - 1;
    else if (cursor_pos < 0)
      cursor_pos = 0;

    if ((abs(old_pos - cursor_pos) != 5 &&
         ((key == SDLK_J) || (key == SDLK_K))))
      cursor_pos = old_pos;
    break;
  }

  return SDL_APP_CONTINUE;
}
// Draw
SDL_AppResult Draw_Image(void *appstate) {
  if (!need_redraw) {
    // SDL_Event e;
    // if (SDL_WaitEventTimeout(&e, 1000)) {
    //   SDL_PushEvent(&e);
    // }
    SDL_Delay(16);
    return SDL_APP_CONTINUE;
  }
  int w, h;
  SDL_GetWindowSize(window, &w, &h);

  SDL_Rect viewport;

  viewport.x = 0;
  viewport.y = 0;
  viewport.w = original_width;
  viewport.h = original_height - 20 - 20;

  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
  SDL_SetRenderViewport(renderer, &viewport);
  SDL_RenderClear(renderer);

  if (full_Textures[cursor_pos] == NULL) {
    Draw_StatusBar(appstate);
    SDL_RenderPresent(renderer);
    if (full_Surfaces[cursor_pos] == NULL) {
      char *path;
      SDL_asprintf(&path, "%s%s", base_path, filenames[cursor_pos]);
      assert(path != NULL);
      full_Surfaces[cursor_pos] = IMG_Load(path);
      if (full_Surfaces[cursor_pos] == NULL) {
        return SDL_APP_CONTINUE;
      }
      full_Textures[cursor_pos] =
          SDL_CreateTextureFromSurface(renderer, full_Surfaces[cursor_pos]);
      if (full_Textures[cursor_pos] == NULL) {
        return SDL_APP_CONTINUE;
      }
    } else {
      return SDL_APP_CONTINUE;
    }
  }

  SDL_FRect dst_rect = {0, 0, full_Textures[cursor_pos]->w,
                        full_Textures[cursor_pos]->h};
  SDL_RectToFRect(&viewport, &dst_rect);

  // float ratio = dst_rect.w / full_Textures[cursor_pos]->w;
  // OLD: dst_rect.h = gTextures[cursor_pos]->h * ratio;

  dst_rect.x = (original_width - dst_rect.w) / 2.0f;
  dst_rect.y = (original_height - dst_rect.h) / 2.0f;

  SDL_RenderTexture(renderer, full_Textures[cursor_pos], NULL, &dst_rect);

  Draw_StatusBar(appstate);
  SDL_RenderPresent(renderer);
  need_redraw = false;
  return SDL_APP_CONTINUE;
}
SDL_AppResult Draw_Thumbnails(void *appstate) {
  if (!need_redraw) {
    // SDL_Event e;
    // if (SDL_WaitEventTimeout(&e, 1000)) {
    //   SDL_PushEvent(&e);
    // }
    SDL_Delay(16);
    return SDL_APP_CONTINUE;
  }
  int w, h;
  SDL_GetWindowSize(window, &w, &h);

  SDL_Rect viewport;
  SDL_FRect dst_rect;
  SDL_FRect cell_rect;

  viewport.x = 10;
  viewport.y = 10;
  viewport.w = original_width - 20;
  viewport.h = original_height - 20 - 20; // Additional 20 for the bar

  float cell_width = (float)(original_width - 20) / 5;
  float cell_height = (float)(original_height - 20) / 4;

  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  Draw_StatusBar(appstate);
  SDL_SetRenderViewport(renderer, &viewport);

  int img_pos = 0;

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 5; ++j) {
      offset_pos = ceilf(((float)cursor_pos + 1) / 5) - 4;
      offset_pos = offset_pos <= 0 ? 0 : offset_pos;
      img_pos = j + (5 * i + (5 * offset_pos));
      if (img_pos >= filenames_size) {
        continue;
      }
      if (thumbnail_Textures[img_pos] == NULL) {
        if (thumbnail_Surfaces[img_pos] != NULL) {
          need_redraw = true;
          thumbnail_Textures[img_pos] = SDL_CreateTextureFromSurface(
              renderer, thumbnail_Surfaces[img_pos]);
          SDL_SetTextureScaleMode(thumbnail_Textures[img_pos],
                                  SDL_SCALEMODE_LINEAR);
          if (thumbnail_Textures[img_pos] == NULL) {
            continue;
          }
        }
        continue;
      }

      cell_rect.x = (int)(j * cell_width);
      cell_rect.y = (int)(i * cell_height);
      cell_rect.w = (int)cell_width;
      cell_rect.h = (int)cell_height;

      // 3. Fixed SDL3 texture size retrieval
      float tex_w, tex_h;
      SDL_GetTextureSize(thumbnail_Textures[img_pos], &tex_w, &tex_h);

      dst_rect.w = tex_w;
      dst_rect.h = tex_h;
      // dst_rect.w = cell_rect.w - 8;
      // float ratio = dst_rect.w / tex_w;
      // dst_rect.h = tex_h * ratio;

      // Center the image in the cell
      dst_rect.x = cell_rect.x + 4;
      dst_rect.y = cell_rect.y + 4;
      cell_rect.w = dst_rect.w + 8;
      cell_rect.h = dst_rect.h + 8;

      // Highlight
      if (cursor_pos == img_pos) {
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderFillRect(renderer, &cell_rect);
      }

      // Render the texture
      SDL_RenderTexture(renderer, thumbnail_Textures[img_pos], NULL, &dst_rect);
    }
  }

  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE;
}
SDL_AppResult Draw_StatusBar(void *appstate) {
  SDL_FRect status_rect;

  status_rect.x = 0;
  status_rect.y = original_height - 20;
  status_rect.w = original_width;
  status_rect.h = 20;

  SDL_SetRenderViewport(renderer, NULL);
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, &status_rect);

  char full_path[4096] = {0};

  strcat(full_path, base_path);
  strcat(full_path, filenames[cursor_pos]);

  if (strcmp(full_path, last_path) != 0) {
    SDL_DestroyTexture(text_texture);
    SDL_Surface *text = TTF_RenderText_Blended(
        font, filenames[cursor_pos], 0,
        text_color); /* replace with full_path for full path */
    char *numstatus;
    SDL_asprintf(&numstatus, "[%d:%d]", cursor_pos, filenames_size);
    SDL_Surface *num = TTF_RenderText_Blended(font, numstatus, 0, text_color);
    if (text) {
      text_texture = SDL_CreateTextureFromSurface(renderer, text);
      SDL_DestroySurface(text);
    }
    SDL_Texture *num_texture = NULL;
    if (num) {
      num_texture = SDL_CreateTextureFromSurface(renderer, num);
      SDL_DestroySurface(num);
    }

    if (!text_texture || !num_texture) {
      SDL_Log("Couldn't create text: %s\n", SDL_GetError());
      return SDL_APP_FAILURE;
    }

    SDL_FRect text_dst = {status_rect.x + 5,
                          status_rect.y + (status_rect.h - text_texture->h) / 2,
                          text_texture->w, text_texture->h};
    SDL_FRect num_dst = {status_rect.w - num_texture->w - 10,
                         status_rect.y + (status_rect.h - text_texture->h) / 2,
                         num_texture->w, num_texture->h};
    SDL_RenderTexture(renderer, text_texture, NULL, &text_dst);
    SDL_RenderTexture(renderer, num_texture, NULL, &num_dst);
  }

  return SDL_APP_CONTINUE;
}
