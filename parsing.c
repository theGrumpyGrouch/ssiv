#include "ssiv.h"

void print_usage() { printf("usage: ssiv [-thv] FILES ...\n"); }
void print_version() { printf("ssiv: version 0.0.1 \n"); }

void parse_options(int argc, char **argv) {
  /* Most of this logic is "reworked" from sxiv, options.c */

  int opt;
  const char *programe_name = strrchr(argv[0], '/');
  programe_name = programe_name ? programe_name + 1 : argv[0];

  while ((opt = getopt(argc, argv, "t::h::v::")) != -1) {
    switch (opt) {
    case '?':
      print_usage();
      exit(EXIT_FAILURE);
    case 'h':
      print_usage();
      exit(EXIT_SUCCESS);
    case 'v':
      print_version();
      exit(EXIT_SUCCESS);
    case 't':
      g_state = THUMBNAILER;
      break;
    }
  }

  paths = argv + optind;
  pathcount = argc - optind;

  if (pathcount == 1 && (strcmp(paths[0], "-") == 0)) {
    paths++;
    pathcount--;
  }

  photos = malloc(sizeof(photo_album_t) * pathcount);

  for (int i = 0; i < pathcount; i++) {
    photos[i].absolute_path = paths[i];
    photos[i].filename = NULL;
    photos[i].filenames = NULL;
    photos[i].filenames_size = 0;
    photos[i].filenames_capacity = 0;
  }
}
