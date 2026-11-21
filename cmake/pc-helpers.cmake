# PC (Windows / MSVC) 構成用の設定と補助関数

set(CMAKE_C_STANDARD 11)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
if(MSVC)
  add_compile_options(/utf-8 /W3)      # ソースは UTF-8。警告は標準レベル
endif()

# add_pc_program(<name> <sources...>)  実行ファイルを作り、共用 UART ライブラリをリンクする
function(add_pc_program name)
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} PRIVATE sea_pc_uart)
endfunction()

# add_comm_auto()
#   PC とボードが通信するレッスンのフォルダ用。
#   pc 構成では、pc/*.c を <フォルダ名>_pc としてビルドする (board/ は無視)
function(add_comm_auto)
  get_filename_component(base "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
  file(GLOB srcs CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/pc/*.c")
  if(NOT srcs)
    message(WARNING "${base}: pc/ に .c ファイルが見つかりません")
    return()
  endif()
  add_pc_program(${base}_pc ${srcs})
endfunction()

# add_avr_lesson_auto()  ボード側だけのレッスンは、pc 構成では何もしない
function(add_avr_lesson_auto)
endfunction()

# target WIN32 to make GUI
function(add_win32_gui_executable target_name)
  add_executable(${target_name} WIN32 ${ARGN})
  if(MSVC)
    target_link_options(${target_name} PRIVATE "/ENTRY:mainCRTStartup")
  endif()
endfunction()
function(add_comm_gui_auto)
  get_filename_component(base "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
  file(GLOB srcs CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/pc/*.c")
  if(NOT srcs)
    message(WARNING "${base}: pc/ に .c ファイルが見つかりません")
    return()
  endif()
  add_executable(${base} WIN32 ${srcs})
  target_link_libraries(${base} PRIVATE sea_pc_uart)
  if(MSVC)
    target_link_options(${base} PRIVATE "/ENTRY:mainCRTStartup")
  endif()
endfunction()