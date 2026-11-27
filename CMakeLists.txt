cmake_minimum_required(VERSION 3.21)

project(cbimgv
  VERSION 0.1.0
  LANGUAGES CXX
)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(cbimgv
  src/main.cpp
  src/imagenavigator.h
  src/imagenavigator.cpp
  src/mainwindow.h
  src/mainwindow.cpp
  src/imageview.h
  src/imageview.cpp
)

target_link_libraries(cbimgv PRIVATE
  Qt6::Widgets
)

install(TARGETS cbimgv
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES data/cbimgv.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

install(FILES data/icons/hicolor/scalable/apps/cbimgv.svg
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
)

install(FILES LICENSE
  DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/cbimgv
)