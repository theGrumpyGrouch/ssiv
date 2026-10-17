<img src="logo.png" alt="logo" width="200">

**Simple SDL Image Viewer**

This project draws inspiration from sxiv, and in that spirit, ssiv is meant to be the perfect image viewer for its creator (me).

Features
--------

* Thumbnail Mode: preview grid of selectable images
* Ability to set the wallpaper using exchangeable backends
* Vi bindings for movement
* Caching for fast reloads

Screenshots
-----------
***Thumbnailer Gallery Mode***: 
<br>
<img src="screenshots/example1.png" alt="example1 " width="540">
<br>
***Image Mode***:
<br>
<img src="screenshots/example2.png" alt="example2 " width="540">

Dependencies
------------

ssiv requires the following to be installed:
```
    * sdl3
    * sdl3_image
    * sdl3_TTF
```

Building
--------

ssiv is built using the following commands:
```bash
    $ make
    $ make install
```

NOTE: The default path is /usr/bin/ssiv.
