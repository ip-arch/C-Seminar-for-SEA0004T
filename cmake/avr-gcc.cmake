# Microchip AVR 8-bit Toolchain (avr-gcc) 用ツールチェーンファイル
# AVR_ROOT は CMakePresets.json で指定します(展開したフォルダ)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

set(AVR_ROOT "C:/avr8-gnu-toolchain-win32_x86_64" CACHE PATH "AVR toolchain folder")
set(AVR_MCU  "atmega328p" CACHE STRING "Target MCU")

# コンパイラ確認用の内部プロジェクトにも AVR_ROOT / AVR_MCU を渡す
set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES AVR_ROOT AVR_MCU)

find_program(CMAKE_C_COMPILER NAMES avr-gcc
             HINTS "${AVR_ROOT}/bin" NO_DEFAULT_PATH REQUIRED)

# リンクまで行わずコンパイラの動作確認だけをする
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# コンパイルとリンクの両方に -mmcu を渡す
set(CMAKE_C_FLAGS_INIT "-mmcu=${AVR_MCU}")
