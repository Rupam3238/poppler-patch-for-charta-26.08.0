# Install script for directory: /home/workstation/tools/deps/poppler-26.08.0/qt6/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/workstation/tools/deps/poppler-26.08")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libpoppler-qt6.so.3.12.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libpoppler-qt6.so.3"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "\$ORIGIN")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/workstation/tools/deps/poppler-26.08.0/build/qt6/src/libpoppler-qt6.so.3.12.0"
    "/home/workstation/tools/deps/poppler-26.08.0/build/qt6/src/libpoppler-qt6.so.3"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libpoppler-qt6.so.3.12.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libpoppler-qt6.so.3"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/workstation/tools/deps/poppler-26.08.0/build:"
           NEW_RPATH "\$ORIGIN")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/workstation/tools/deps/poppler-26.08.0/build/qt6/src/libpoppler-qt6.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libpoppler-qt6.so.3.12.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libpoppler-qt6.so.3"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "\$ORIGIN")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/workstation/tools/deps/poppler-26.08.0/build/qt6/src/libpoppler-qt6.so.3.12.0"
    "/home/workstation/tools/deps/poppler-26.08.0/build/qt6/src/libpoppler-qt6.so.3"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libpoppler-qt6.so.3.12.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libpoppler-qt6.so.3"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/workstation/tools/deps/poppler-26.08.0/build:"
           NEW_RPATH "\$ORIGIN")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/workstation/tools/deps/poppler-26.08.0/build/qt6/src/libpoppler-qt6.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/poppler/qt6" TYPE FILE FILES
    "/home/workstation/tools/deps/poppler-26.08.0/qt6/src/poppler-qt6.h"
    "/home/workstation/tools/deps/poppler-26.08.0/qt6/src/poppler-link.h"
    "/home/workstation/tools/deps/poppler-26.08.0/qt6/src/poppler-annotation.h"
    "/home/workstation/tools/deps/poppler-26.08.0/qt6/src/poppler-form.h"
    "/home/workstation/tools/deps/poppler-26.08.0/qt6/src/poppler-optcontent.h"
    "/home/workstation/tools/deps/poppler-26.08.0/qt6/src/poppler-page-transition.h"
    "/home/workstation/tools/deps/poppler-26.08.0/qt6/src/poppler-media.h"
    "/home/workstation/tools/deps/poppler-26.08.0/qt6/src/poppler-converter.h"
    "/home/workstation/tools/deps/poppler-26.08.0/build/qt6/src/poppler-export.h"
    "/home/workstation/tools/deps/poppler-26.08.0/build/qt6/src/poppler-version.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/workstation/tools/deps/poppler-26.08.0/build/qt6/src/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
