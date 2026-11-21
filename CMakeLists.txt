cmake_minimum_required(VERSION 3.20)

# ビルド対象: avr = ボード側 (ATmega328P)、pc = Windows 側 (MSVC)。CMakePresets.json の構成で切り替わる
set(SEA_TARGET "avr" CACHE STRING "ビルド対象: avr (ボード側) / pc (Windows 側)")

project(SEA0004T_Seminar C)

if(SEA_TARGET STREQUAL "avr")
  include(cmake/avr-helpers.cmake)
  add_subdirectory(bsp)         # ボード用ライブラリ
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
else()
  include(cmake/pc-helpers.cmake)
  add_subdirectory(pc_lib)      # PC 用の共用 UART ライブラリ
endif()

# 次の名前のフォルダは、自動でビルド対象になる (どちらの構成でも同じ)
#   lesson_*  : ボード側だけのレッスン (pc 構成では無視される)
#   comm_*    : PC とボードが通信するレッスン (board/ と pc/ をもつ)
#   work_*    : 受講生が、上のフォルダをコピーして作る課題
file(GLOB folders RELATIVE "${CMAKE_SOURCE_DIR}" CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/lesson_*"
     "${CMAKE_SOURCE_DIR}/work_*"
     "${CMAKE_SOURCE_DIR}/comm_*"
     "${CMAKE_SOURCE_DIR}/windows_gui/lesson_gui_*"
     "${CMAKE_SOURCE_DIR}/windows_gui/lesson_*"
     "${CMAKE_SOURCE_DIR}/windows_gui/work_*")
list(SORT folders)
foreach(dir IN LISTS folders)
  if(NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/${dir}")
    continue()
  endif()
  if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${dir}/CMakeLists.txt")
    message(WARNING "${dir}: CMakeLists.txt がありません。フォルダごとコピーしましたか?")
    continue()
  endif()
  if(NOT dir MATCHES "^[A-Za-z0-9_/]+$")
    message(WARNING "フォルダ名 '${dir}' は、半角の英数字と _ だけにしてください。このフォルダは無視します")
    continue()
  endif()
  add_subdirectory(${dir})
endforeach()
