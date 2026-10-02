#include "ssiv.h"
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#define NUM_THREADS 8

SDL_AppResult find_files(int argc, char *argv[]) {
  for (int i = 0; i < pathcount; i++) {
    char* base_path = paths[i];
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

    DIR *dp = opendir(paths[i]);

    if (dp == NULL) {
      printf("sxiv: Directory could not be opened\n %s\n", argv[i + 1]);
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

      if (photos[i].filenames_size >= photos[i].filenames_capacity) {
        photos[i].filenames_capacity = (photos[i].filenames_capacity == 0)
                                           ? 1
                                           : photos[i].filenames_capacity * 2;
        photos[i].filenames = realloc(
            photos[i].filenames, sizeof(char *) * photos[i].filenames_capacity);
      }

      photos[i].filenames[photos[i].filenames_size] = strdup(entry->d_name);
      photos[i].filenames_size++;
    }

    closedir(dp);

    if (photos[i].filenames_size == 0) {
      printf("ssiv: No Images to Display\n");
      return SDL_APP_FAILURE;
    }
  }

  return SDL_APP_CONTINUE;
}

void *load_thumbnail_Surface(void *args) {
  int_tuple_t img_pos = *((int_tuple_t *)args);

  int surface_pos = img_pos.k;
  for (int h = 0; h < img_pos.i; h++) {
    surface_pos += photos[h].filenames_size;
  }

  if (img_pos.k >= photos[img_pos.i].filenames_size) {
    return NULL;
  }

  char *userPath = getenv("HOME");
  char *fullpath;
  if (SDL_asprintf(&fullpath, "%s/.cache/ssiv/%s%s", userPath,
                   photos[img_pos.i].absolute_path,
                   photos[img_pos.i].filenames[img_pos.k]) < 0) {
    printf("sxiv: allocation failed\n");
    return NULL;
  }

  char *path;
  SDL_asprintf(&path, "%s%s", photos[img_pos.i].absolute_path,
               photos[img_pos.i].filenames[img_pos.k]);

  if (path == NULL) {
    printf("sxiv: allocation failed\n");
    return NULL;
  }

  if (SDL_IOFromFile(fullpath, "r") != NULL) { // Cache of thumbnail exists
    struct stat file_status;
    struct stat cache_file_status;

    stat(path, &file_status);
    stat(fullpath, &cache_file_status);

    if (file_status.st_mtim.tv_sec > cache_file_status.st_mtim.tv_sec ||
        file_status.st_ctim.tv_sec > cache_file_status.st_ctim.tv_sec) {
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
    thumbnail_Surfaces[surface_pos] = tmp;
    return NULL;
  }

SCALE:
  if (thumbnail_Surfaces[surface_pos] != NULL) {
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

  thumbnail_Surfaces[surface_pos] =
      scale_surface_stb(tmp, new_width, new_height, img_pos);

  if (thumbnail_Surfaces[surface_pos] == NULL) {
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

  for (int index = 0; index < pathcount; index++) {
    for (int i = 0; i < photos[index].filenames_size; i++) {
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

      int_tuple_t *img_pos = malloc(sizeof(int_tuple_t));
      img_pos->i = index;
      img_pos->k = i;
      pthread_create(&threads[active_threads], NULL, load_thumbnail_Surface,
                     (void *)img_pos);
      active_threads++;
    }
  }

  return NULL;
}
