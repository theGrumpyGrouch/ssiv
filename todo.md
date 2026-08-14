# GMPC - GRAPHICAL MUSIC PLAYER CONTROLLER

GMPC is a graphical frontend for the music player daemon.
This is achieved with the coupling a graphical framework and mpc.

## Objectives
* A graphical frontend that allows for a user to select and play music
* Album cover recognition from flac, png, or jpg files
* fast startup & low memory usage
* Caching of images

## TODO
[X] - Select a graphical framework (Qt, SDL, etc...) 
[~] - Image thumbnailer for album covers (esiv: Extensible Sdl Image Viewer)
[] - Tracklist frontend 
[] - Queue frontend 
[] - map mpc api calls to track selection
[] - extract flac cover files with taglib or other

## Thumbnailer 
* Cache images after rezised and made billinear.
    * Use scale_surface_bilinear in find_files to generate cache
    * change the way size of images is handled from being in ui.c to filehandling.c
        * SDL_RenderTexture() always resizes using nearest neigbhor, so we can't have that. 

## Concept 
<pre>
┌──────────────────────────────────────────────┐
│              Your Image Viewer               │
├──────────────────────┬───────────────────────┤
│   Main Window (800x600)                      │
│   ┌──────────────────┐  ┌──────────────────┐ │
│   │  Large Preview   │  │  Thumbnail Grid  │ │
│   │  (selected img)  │  │  ┌──┐┌──┐┌──┐    │ │
│   │                  │  │  │  ││  ││  │    │ │
│   │                  │  │  └──┘└──┘└──┘    │ │
│   └──────────────────┘  └──────────────────┘ │
└──────────────────────────────────────────────┘
</pre>



