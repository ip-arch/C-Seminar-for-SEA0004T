# AVR 向けの共通設定と、lesson 用の補助関数

set(AVR_PORT "ch340"  CACHE STRING "avrdude で使うポート。デバイス名 (ch340) か COM 番号 (例: COM3)")
set(AVR_BAUD "115200" CACHE STRING "書き込み速度 (旧ブートローダなら 57600)")

find_program(AVR_OBJCOPY NAMES avr-objcopy HINTS "${AVR_ROOT}/bin" NO_DEFAULT_PATH REQUIRED)
find_program(AVR_SIZE    NAMES avr-size    HINTS "${AVR_ROOT}/bin" NO_DEFAULT_PATH REQUIRED)

# tools/avrdude に同梱した avrdude を優先し、無ければ PATH から探す
find_program(AVRDUDE NAMES avrdude HINTS "${CMAKE_SOURCE_DIR}/tools/avrdude")
if(AVRDUDE)
  if(EXISTS "${CMAKE_SOURCE_DIR}/tools/avrdude/avrdude.conf")
    set(AVRDUDE_CONF -C "${CMAKE_SOURCE_DIR}/tools/avrdude/avrdude.conf")
  endif()
else()
  message(STATUS "avrdude が見つかりません。tools/avrdude に avrdude.exe と avrdude.conf を置いてください(upload_* を実行するとエラーになります。HEX の生成は可能)")
endif()

# --- シリアルモニタ (Windows のみ) ---------------------------------------
#   monitor ターゲット : printf の出力を別ウィンドウに表示する
#   upload_* ターゲット: 書き込み前に、開いているモニタを自動で閉じる (COM ポートの解放)
if(CMAKE_HOST_WIN32)
  find_program(POWERSHELL NAMES powershell)
endif()
if(POWERSHELL)
  set(SERIAL_TOOLS "${CMAKE_SOURCE_DIR}/tools/serial")
  set(MONITOR_PID  "${CMAKE_BINARY_DIR}/monitor.pid")
  set(PS_RUN ${POWERSHELL} -NoProfile -ExecutionPolicy Bypass -File)

  add_custom_target(monitor
    COMMAND ${PS_RUN} "${SERIAL_TOOLS}/launch-monitor.ps1"
            -Port "${AVR_PORT}" -Baud 115200 -PidFile "${MONITOR_PID}"
    COMMENT "シリアルモニタを別ウィンドウで開く (${AVR_PORT}, 115200bps)"
    VERBATIM)

  set(PRE_UPLOAD COMMAND ${PS_RUN} "${SERIAL_TOOLS}/stop-monitor.ps1" -PidFile "${MONITOR_PID}")
endif()

# 全レッスン共通のコンパイル設定
# 最適化はサイズ優先(-Os)に統一する。CMake 既定の -O3 は使わない
set(CMAKE_C_FLAGS_RELEASE "")

# --- Visual Studio の IntelliSense (F12: 定義へ移動 など) のための設定 -------------
#   avr-gcc は、-mmcu=atmega328p から、次のものを内部で自動的に決めている。
#   IntelliSense はそれを知らないので、コンパイラに問い合わせて、同じ値を渡す。
#     1. 標準のヘッダの場所 (avr-libc、gcc 内蔵の stddef.h など)
#     2. -mmcu から暗黙に決まるマクロ (__AVR_ATmega328P__ など)
#        avr/io.h は、これを手がかりに、読み込むデバイス用ヘッダ (iom328p.h) を選ぶ。
#        無いと、PORTB や DDRB などが「未定義」になり、F12 で飛べない
#   どちらも、コンパイラがすでに使っている値と同じものなので、実際のビルド結果は変わらない。
set(AVR_SYSTEM_INCLUDES "")
set(AVR_BUILTIN_DEFINES "")
set(_avr_empty "${CMAKE_BINARY_DIR}/avr_query_empty.c")
file(WRITE "${_avr_empty}" "")

execute_process(
  COMMAND "${CMAKE_C_COMPILER}" -mmcu=${AVR_MCU} -x c -E -v "${_avr_empty}"
          -o "${CMAKE_BINARY_DIR}/avr_query_empty.i"
  ERROR_VARIABLE _avr_verbose OUTPUT_QUIET RESULT_VARIABLE _avr_rc)
if(_avr_rc EQUAL 0)
  string(REPLACE "\r" "" _avr_verbose "${_avr_verbose}")
  string(REGEX MATCH "#include <\\.\\.\\.> search starts here:\n(.*)\nEnd of search list" _m "${_avr_verbose}")
  string(REPLACE "\n" ";" _avr_dirs "${CMAKE_MATCH_1}")
  foreach(_d IN LISTS _avr_dirs)
    string(STRIP "${_d}" _d)
    if(_d AND IS_DIRECTORY "${_d}")
      get_filename_component(_d "${_d}" REALPATH)
      list(APPEND AVR_SYSTEM_INCLUDES "${_d}")
    endif()
  endforeach()
endif()

execute_process(
  COMMAND "${CMAKE_C_COMPILER}" -mmcu=${AVR_MCU} -dM -E -x c "${_avr_empty}"
  OUTPUT_VARIABLE _avr_macros ERROR_QUIET RESULT_VARIABLE _avr_rc)
if(_avr_rc EQUAL 0)
  string(REPLACE "\r" "" _avr_macros "${_avr_macros}")
  string(REPLACE "\n" ";" _avr_lines "${_avr_macros}")
  foreach(_l IN LISTS _avr_lines)
    if(_l MATCHES "^#define (__AVR[A-Za-z0-9_]*) ([A-Za-z0-9_]+)$")
      list(APPEND AVR_BUILTIN_DEFINES "${CMAKE_MATCH_1}=${CMAKE_MATCH_2}")
    endif()
  endforeach()
endif()
if(NOT AVR_SYSTEM_INCLUDES OR NOT AVR_BUILTIN_DEFINES)
  message(STATUS "IntelliSense 用の情報をコンパイラから取得できませんでした (ビルドには影響しません)")
endif()

# CMake は、コンパイラが標準で探すヘッダの場所を、-I の一覧から自動的に取り除く。
# すると IntelliSense にもその場所が渡らないので、取り除かないようにする。
# (コンパイラは、標準の場所を -I で重ねて指定されても無視するので、ビルドには影響しない)
set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES "")

add_library(avr_common INTERFACE)
target_include_directories(avr_common INTERFACE ${AVR_SYSTEM_INCLUDES})
target_compile_definitions(avr_common INTERFACE ${AVR_BUILTIN_DEFINES})
target_compile_options(avr_common INTERFACE
  -Os -Wall -Wextra -std=gnu11 -ffunction-sections -fdata-sections)
target_compile_definitions(avr_common INTERFACE F_CPU=16000000UL)
target_link_options(avr_common INTERFACE -Wl,--gc-sections)

# add_avr_lesson(<name> <sources...>)
#   <name>.elf と <name>.hex を作る。avrdude があれば upload_<name> も作る
function(add_avr_lesson name)
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} PRIVATE avr_common)
  set_target_properties(${name} PROPERTIES SUFFIX ".elf")

  add_custom_command(TARGET ${name} POST_BUILD
    COMMAND ${AVR_OBJCOPY} -O ihex -R .eeprom
            $<TARGET_FILE:${name}> $<TARGET_FILE_DIR:${name}>/${name}.hex
    COMMAND ${AVR_SIZE} -B $<TARGET_FILE:${name}>
    COMMENT "HEX を生成: ${name}.hex")

  if(AVRDUDE)
    add_custom_target(upload_${name}
      ${PRE_UPLOAD}
      COMMAND ${AVRDUDE} ${AVRDUDE_CONF} -p m328p -c arduino
              -P ${AVR_PORT} -b ${AVR_BAUD} -D
              -U flash:w:$<TARGET_FILE_DIR:${name}>/${name}.hex:i
      DEPENDS ${name}
      USES_TERMINAL
      COMMENT "${name}.hex を ${AVR_PORT} へ書き込み")
  else()
    # avrdude がなくても upload_* は表示し、実行したときに原因を知らせて止まる
    add_custom_target(upload_${name}
      COMMAND ${CMAKE_COMMAND} -E echo "avrdude が見つかりません。tools/avrdude に avrdude.exe と avrdude.conf を置いて、CMake キャッシュを削除して再構成してください"
      COMMAND ${CMAKE_COMMAND} -E false
      VERBATIM)
  endif()
endfunction()

# add_avr_lesson_auto()
#   各 lesson / 課題フォルダの CMakeLists.txt に、この 1 行だけを書く。
#   ・ターゲット名は、フォルダの名前になる (フォルダを複製して名前を変えるだけで新しい課題になる)
#   ・フォルダ内の *.c は、すべてビルド対象になる (ファイルを足しても CMakeLists.txt は変更不要)
#   ・ボード用ライブラリ (bsp) は自動でリンクされる
function(add_avr_lesson_auto)
  get_filename_component(name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
  file(GLOB srcs CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
  if(NOT srcs)
    message(WARNING "${name}: .c ファイルが見つかりません。このフォルダは無視します")
    return()
  endif()
  add_avr_lesson(${name} ${srcs})
  target_link_libraries(${name} PRIVATE sea0004t_bsp)
endfunction()

# add_comm_auto()
#   PC とボードが通信するレッスンのフォルダ (board/ と pc/ をもつ) 用。
#   avr 構成では、board/*.c を <フォルダ名>_board としてビルドする (pc/ は無視)
function(add_comm_auto)
  get_filename_component(base "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
  file(GLOB srcs CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/board/*.c")
  if(NOT srcs)
    message(WARNING "${base}: board/ に .c ファイルが見つかりません")
    return()
  endif()
  add_avr_lesson(${base}_board ${srcs})
  target_link_libraries(${base}_board PRIVATE sea0004t_bsp)
endfunction()
