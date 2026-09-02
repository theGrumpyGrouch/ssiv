#include "ssiv.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#define NUM_THREADS 8

SDL_AppResult find_files() {
  int is_file = 0;
  struct stat s;
  if (stat(base_path, &s) == 0) {
    if (s.st_mode & S_IFREG) {
      is_file = 1;
      char *p1 = strrchr(base_path, '/');
      char *p2 = strrchr(p1, '/');
      if (p1 == NULL || p2 == NULL) {
        printf("ssiv: Invalid path format\n");
        return SDL_APP_FAILURE;
      }
      *p2 = '\0';
    } else if (s.st_mode & S_IFDIR) {
    } else {
      printf("ssiv: Invalid path\n");
      return SDL_APP_FAILURE;
    }
  } else {
    printf("ssiv: stat failed, %s, %s\n", base_path, strerror(errno));
    return SDL_APP_FAILURE;
  }

  if (base_path[strlen(base_path) - 1] != '/')
    strcat(base_path, "/");

  DIR *dp = opendir(base_path);

  if (dp == NULL) {
    printf("sxiv: Directory could not be opened\n %s\n", base_path);
    return SDL_APP_FAILURE;
  }

  struct dirent *entry;

  while ((entry = readdir(dp)) != NULL) {
    if (entry->d_type == DT_DIR) {
      continue;
    }

    const char *ext = strrchr(entry->d_name, '.');
    if (!ext || !(strcmp(ext, ".png") == 0 || strcmp(ext, ".jpg") == 0 ||
                  strcmp(ext, ".jpeg") == 0))
      continue;

    if (filenames_size >= filenames_capacity) {
      filenames_capacity =
          (filenames_capacity == 0) ? 1 : filenames_capacity * 2;
      filenames = realloc(filenames, sizeof(char *) * filenames_capacity);
    }

    filenames[filenames_size] = strdup(entry->d_name);
    filenames_size++;
  }

  closedir(dp);

  if (filenames_size == 0) {
    printf("ssiv: No Images to Display\n");
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

void *load_thumbnail_Surface(void *args) {
  int img_pos = *((int *)args);

  if (img_pos >= filenames_size) {
    return NULL;
  }

  char *userPath = getenv("HOME");
  char *fullpath;
  if (SDL_asprintf(&fullpath, "%s/.cache/ssiv/%s%s", userPath, base_path,
                   filenames[img_pos]) < 0) {
    printf("sxiv: allocation failed\n");
    return NULL;
  }

  char *path;
  SDL_asprintf(&path, "%s%s", base_path, filenames[img_pos]);

  if (path == NULL) {
    printf("sxiv: allocation failed\n");
    return NULL;
  }

  if (SDL_IOFromFile(fullpath, "r") != NULL) { // Cache of thumbnail exists
    struct stat file_status;
    struct stat cache_file_status;

    stat(path, &file_status);
    stat(fullpath, &cache_file_status);

    if (file_status.st_mtim.tv_sec > cache_file_status.st_mtim.tv_sec 
        || file_status.st_ctim.tv_sec > cache_file_status.st_ctim.tv_sec) {
      goto SCALE;
    }

    SDL_Surface *tmp = IMG_Load(fullpath);
    tmp = SDL_ConvertSurface(tmp, SDL_PIXELFORMAT_RGBA8888);

    if (tmp == NULL) {
      printf("sxiv: surface creation failed 1, %s\n", path);
      return NULL;
    }

    free(path);
    free(fullpath);
    thumbnail_Surfaces[img_pos] = tmp;
    return NULL;
  }

SCALE:
  if (thumbnail_Surfaces[img_pos] != NULL) {
    printf("sxiv: thumbnail surfaces uninitialized, %s\n", path);
    return NULL;
  }

  SDL_Surface *tmp = IMG_Load(path);
  tmp = SDL_ConvertSurface(tmp, SDL_PIXELFORMAT_RGBA8888);
  if (tmp == NULL) {
    printf("sxiv: surface creation failed 3, %s\n", path);
    return NULL;
  }

  int new_width = ((float)(original_width - 20) / 5) - 8;
  float ratio = (float)new_width / tmp->w;
  int new_height = tmp->h * ratio;

  thumbnail_Surfaces[img_pos] =
      scale_surface_stb(tmp, new_width, new_height, img_pos);

  if (thumbnail_Surfaces[img_pos] == NULL) {
    printf("sxiv: surface creation failed\n");
    return NULL;
  }

  free(path);
  free(fullpath);
  return NULL;
}

void *render_threads(void *arg) {
  pthread_t threads[NUM_THREADS];
  int active_threads = 0;

  for (int i = 0; i < filenames_size; i++) {
    while (active_threads >= NUM_THREADS) {
      for (int j = 0; j < 8; j++) {
        if (threads[j] != 0) { // Assuming 0 means "not in use"
          pthread_join(threads[j], NULL);
          threads[j] = 0;
          active_threads--;
          break;
        }
      }
    }

    int *img_pos = malloc(sizeof(int));
    *img_pos = i;
    pthread_create(&threads[active_threads], NULL, load_thumbnail_Surface,
                   (void *)img_pos);
    active_threads++;
  }

  return NULL;
}
