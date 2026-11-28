# cbimgv

cbimgv is a lightweight image viewer for Linux desktops, built with Qt 6
Widgets. Open an image directly from a file manager or the command line, then
view and navigate images in the same directory.

## Features

- Fit images to the window, view them at original size, zoom, and pan.
- Navigate through supported images in the current directory, with natural
  filename sorting and wraparound navigation.
- Browse nearby files with an optional thumbnail ribbon.
- Show the filename, image dimensions, and zoom level in a compact status area.
- Refresh the directory listing when its contents change.
- Drag an image to another application or copy the image and its file
  reference to the clipboard.
- Move an image to Trash or permanently delete it from the viewer.
- Integrate with Linux desktops through a freedesktop-compatible application
  entry and icon.

Image formats depend on the Qt image-format plugins installed on the system.
Qt's built-in formats cover common types; additional formats, including SVG,
WebP, and TIFF, may require the corresponding Qt plugin packages.

## Usage

Open an image from the command line:

```sh
cbimgv /path/to/image.jpg
```

The application can also be selected as the handler for image files in a
desktop file manager. The included desktop entry advertises common image MIME
types. Choosing cbimgv as the default handler is controlled by the desktop
environment and does not change existing file associations automatically.

## Controls

| Input | Action |
| --- | --- |
| Left / Right | Previous / next image; wraps at either end |
| Home / End | First / last image |
| Page Up / Page Down | Move by 10 images; wraps when already at the corresponding end |
| Mouse wheel | Zoom in / out |
| Tab or middle-click | Toggle wheel navigation (up for previous, down for next) |
| Ctrl + mouse wheel | Zoom regardless of the wheel-navigation mode |
| Left / right edge click | Previous / next image |
| Drag | Drag the current image to another application when it is not being panned |
| Ctrl + C | Copy the image and its file reference |
| F | Fit image to the window |
| 1 | Show image at original size (100%) |
| Double-click | Toggle between fit-to-window and original size |
| Plus / Minus | Zoom in / out |
| G | Toggle the thumbnail ribbon |
| S | Toggle the status area |
| F11 | Toggle fullscreen |
| Delete | Move the current image to Trash |
| Shift + Delete | Permanently delete the current image |
| Esc, Q, or right-click | Close cbimgv |

The footer also contains buttons for several viewing controls. Deletion by
Shift + Delete is permanent.

## Build

Requirements:

- C++17 compiler
- CMake 3.21 or newer
- Ninja
- Qt 6 Widgets development files

Configure and build from the repository root:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Run the locally built application:

```sh
./build/cbimgv /path/to/image.jpg
```

## Install

To install into a user-local prefix:

```sh
cmake --install build --prefix "$HOME/.local"
```

Ensure `$HOME/.local/bin` is on your `PATH` to launch `cbimgv` by name. To
install system-wide, choose an appropriate prefix such as `/usr` and use the
installation method provided by your distribution. Do not run the build itself
with elevated privileges.

## Arch Linux

An Arch Linux PKGBUILD is maintained in [`packaging/arch/PKGBUILD`](packaging/arch/PKGBUILD).
It builds from a versioned GitHub source archive. The package recipe is not yet
published to the AUR; a tagged source release and a verified source checksum
are required before publication. An AUR submission also needs the packaging
sources' 0BSD license and REUSE metadata, separate from cbimgv's Apache
application license.

## License

cbimgv is distributed under the Apache License 2.0. See [`LICENSE`](LICENSE)
for the complete license text.
