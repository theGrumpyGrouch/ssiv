#include <SDL3/SDL_filesystem.h>
#include <stdio.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "gsiv.h"
#include "libs/stb_image_resize2.h"

int clamp(int val, int min, int max) {
  if (val < min)
    return min;
  else if (val > max)
    return max;
  return val;
}

Uint8 linear_interpolation(Uint8 a, Uint8 b, float t) {
  return floor(a + (t * (b - a)));
}

Uint32 color_linear_interpolate(Uint32 a, Uint32 b, float t) {
  Uint8 a_red_channel = a >> 24;
  Uint8 a_blue_channel = a >> 16;
  Uint8 a_green_channel = a >> 8;
  Uint8 a_alpha_channel = a;

  Uint8 b_red_channel = b >> 24;
  Uint8 b_blue_channel = b >> 16;
  Uint8 b_green_channel = b >> 8;
  Uint8 b_alpha_channel = b;

  Uint8 final_color[4];
  final_color[3] = linear_interpolation(a_red_channel, b_red_channel, t);
  final_color[2] = linear_interpolation(a_green_channel, b_green_channel, t);
  final_color[1] = linear_interpolation(a_blue_channel, b_blue_channel, t);
  final_color[0] = linear_interpolation(a_alpha_channel, b_alpha_channel, t);

  return *(Uint32 *)final_color;
}

Uint32 color_bilinear_interpolation(SDL_Surface *src, float src_x,
                                    float src_y) {
  Uint32 final_color;

  // Four courners (x1, y1) (x2, y1) (x1, y2) (x2, y2)
  int x1 = clamp(floor(src_x), 0, src->w - 1);
  int x2 = clamp(ceil(src_x), 0, src->w - 1);
  int y1 = clamp(floor(src_y), 0, src->h - 1);
  int y2 = clamp(ceil(src_y), 0, src->h - 1);

  float frac_x = src_x - x1;
  float frac_y = src_y - y1;

  // Colors of the four courners
  SDL_LockSurface(src);
  Uint32 *pixels = (Uint32 *)src->pixels;
  int pitch = src->pitch;
  Uint32 top_left = pixels[y1 * (pitch / 4) + x1];
  Uint32 top_right = pixels[y1 * (pitch / 4) + x2];
  Uint32 bottom_left = pixels[y2 * (pitch / 4) + x1];
  Uint32 bottom_right = pixels[y2 * (pitch / 4) + x2];
  SDL_UnlockSurface(src);

  // Interpolate
  Uint32 top_color = color_linear_interpolate(top_left, top_right, frac_x);
  Uint32 bottom_color =
      color_linear_interpolate(bottom_left, bottom_right, frac_x);
  final_color = color_linear_interpolate(top_color, bottom_color, frac_y);

  return final_color;
}

SDL_Surface *scale_surface_bilinear(SDL_Surface *src, int width, int height) {
  SDL_Surface *dst = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA8888);

  SDL_LockSurface(src);
  SDL_LockSurface(dst);
  Uint32 *dst_pixels = (Uint32 *)dst->pixels;
  int dst_pitch = dst->pitch;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      float src_x = ((float)x / (float)(width - 1)) * (src->w - 1);
      float src_y = ((float)y / (float)(height - 1)) * (src->h - 1);
      Uint32 color = color_bilinear_interpolation(src, src_x, src_y);
      dst_pixels[y * (dst_pitch / 4) + x] = color;
    }
  }
  SDL_UnlockSurface(src);
  SDL_UnlockSurface(dst);

  return dst;
}

SDL_Surface *scale_surface_stb(SDL_Surface *src, int width, int height,
                               int pos) {

  SDL_Surface *resized_src =
      SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA8888);

  SDL_LockSurface(resized_src);

  stbir_resize_uint8_linear((Uint8 *)src->pixels, src->w, src->h, src->pitch,
                            (Uint8 *)resized_src->pixels, width, height,
                            resized_src->pitch, STBIR_RGBA_PM);

  SDL_UnlockSurface(resized_src);

  char *userPath = getenv("HOME");
  char *path;

  assert(asprintf(&path, "%s/.cache/gsiv/%s", userPath, base_path));
  assert(SDL_CreateDirectory(path) == true);
  assert(asprintf(&path, "%s%s%s%s", userPath, "/.cache/gsiv/", base_path, filenames[pos]) != 1);
  IMG_SaveJPG(resized_src, path, 100);

  return resized_src;
}
