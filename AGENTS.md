# AGENTS.md

## Project Overview

`cbimgv` is a simple, lightweight, fast image viewer for Linux, designed to behave like a dedicated file-preview utility rather than a photo-management application.

Its intended user experience is similar to the Windows 10 Photos application, with a minimal interface and a focus on quickly viewing and navigating images.

The application is primarily intended to be launched by double-clicking or pressing Enter on image files in KDE Dolphin.

GitHub repository:

- HTTPS: <https://github.com/devbernardolopes/cbimgv.git>
- SSH: git@github.com:devbernardolopes/cbimgv.git

## Target Platform and Technologies

- Target operating system: Linux desktop.
- Primary desktop environment: KDE Plasma / Dolphin.
- Development environment: Omarchy Linux (Arch-based).
- Programming language: C++17.
- GUI framework: Qt 6 Widgets.
- Build system: CMake.
- Build generator: Ninja.
- Compiler: GCC.

Use native Qt 6 functionality wherever possible. Avoid introducing external dependencies unless they provide a clear and necessary benefit.

Do not use Qt Quick, QML, Qt Designer, or `.ui` files. Build the interface programmatically using Qt Widgets.

## Core Requirements

The application should provide the following functionality:

- Open an image directly from a command-line filename argument.
- Display images immediately, without an intermediate interface.
- Navigate between images in the same directory using Left and Right arrow keys.
- Support cyclic navigation (last image to first and vice versa).
- Close the application when ESC is pressed.
- Zoom in and out using the mouse wheel.
- Reset zoom to 100% (original image size) using double-click.
- Support image panning when zoomed in.
- Preserve image aspect ratio.
- Automatically fit images to the available window area.
- Display image dimensions in a permanently visible, unobtrusive status area.

The interface should remain minimal. Avoid unnecessary toolbars, sidebars, panels, menus, dialogs, and other visual elements unless explicitly required.

## Architecture

Keep responsibilities separated into three primary components:

- **MainWindow:** Main application window, window management, keyboard shortcuts, and status bar.
- **ImageView:** Image rendering, scaling, zooming, panning, and mouse interaction.
- **ImageNavigator:** Directory enumeration, natural filename sorting, and image navigation.

Keep `main.cpp` responsible for application initialization, command-line argument handling, and initial image loading.

Avoid concentrating unrelated functionality in a single class. Keep components focused and maintain clear interfaces between them.

## Coding Guidelines

- Use modern C++17 features and conventions.
- Prefer Qt classes and functionality over implementing existing functionality from scratch.
- Follow Qt ownership conventions for widgets and QObject-derived classes.
- Use RAII and avoid unnecessary manual memory management.
- Use descriptive names for classes, functions, and variables.
- Keep functions focused and reasonably small.
- Avoid unnecessary abstractions, inheritance, and overengineering.
- Handle invalid files, unsupported formats, and other expected errors gracefully.
- Preserve existing functionality when introducing new features.
- Do not introduce dependencies or architectural changes without a clear justification.

## Build and Verification

The project must remain buildable using the existing CMake configuration.

Configure the project:

```bash
cmake -S . -B build -G Ninja
```

Build:

```bash
cmake --build build
```

Run:

```bash
./build/cbimgv /path/to/image.jpg
```

After making changes, ensure the project builds successfully and test affected functionality whenever possible.

Keep generated build files and compiled artifacts out of version control.

## Development Priorities

Prioritize:

1. Simplicity.
2. Fast startup and responsive interaction.
3. Low memory and resource consumption.
4. Minimal runtime dependencies.
5. Native Linux desktop integration.
6. Maintainable and well-structured code.

The application should do one thing well: open, display, navigate, and close images quickly and reliably.

Avoid turning `cbimgv` into a photo manager or a general-purpose image editing application.
