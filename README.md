# cbimgv

cbimgv is a lightweight image viewer for Linux. It opens one image directly
from a command-line path and provides minimal viewing and directory navigation
controls.

## Current status

This project is in active development. It currently uses Qt Widgets to open,
display, zoom, pan, and navigate images in the same directory. When the image
does not need panning, it can also be dragged to another application as a file,
with a thumbnail shown beside the cursor. Ctrl+C copies both the image and its
file reference for pasting into other applications.

## Keyboard shortcuts

- Ctrl+C: copy the current image and its file reference.
- Delete: move the current image to Trash.
- Shift+Delete: permanently delete the current image.
- F11: toggle fullscreen.
- Tab: toggle the mouse wheel between zooming and image navigation (up for
  previous, down for next).
- Middle-click (press the scroll wheel): toggle between wheel zoom and image
  navigation.
- F: fit the image to the window.
- 1: show the image at original size (100%).
- Ctrl+mouse wheel: zoom regardless of the current wheel mode.
- G: toggle the thumbnail ribbon.
- S: toggle the status bar.
- Esc: close cbimgv.
- Plus/Minus: zoom in/out.
- Right-click: quit cbimgv.
- Left/Right: move to the previous/next image, wrapping at either end.
- Page Up/Page Down: move back/forward 10 images, clamping to the first or
  last image. From the first/last image, the shortcut wraps by 10 instead.
- Home/End: select the first/last image.

The footer also provides buttons for these shortcuts. Toggle buttons reflect
their current state and can be clicked directly.

## Technologies

- C++17
- Qt 6 Widgets
- CMake
- Ninja

## Build

Configure and build with CMake and Ninja:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Installation

To install into the configured prefix:

```bash
cmake --install build
```

For a staged installation, for example to package for a distribution:

```bash
DESTDIR=/tmp/cbimgv-package cmake --install build
```

For a `/usr` prefix:

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
DESTDIR=/tmp/cbimgv-package cmake --install build
```

Do not use privileged installation commands merely to build or stage the
package.

## Desktop integration

cbimgv provides a freedesktop-compatible desktop entry and application icon.
The desktop entry declares MIME types for common image formats, allowing Linux
desktop environments and file managers to offer cbimgv as an available handler.
The user or desktop environment chooses the default application; installing
cbimgv does not change existing MIME defaults or per-user preferences.

## Arch Linux packaging

An experimental PKGBUILD is prepared under `packaging/arch/`. It is not yet
published to the Arch User Repository (AUR).

## License

The project is intended to be licensed under Apache-2.0. The complete license
text still needs to be inserted into `LICENSE` before this project is treated
as licensed or published as a distributable release.
