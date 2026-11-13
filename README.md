# SEA0004T-Seminar (テンプレート)

Visual Studio + Microchip AVR 8-bit Toolchain (avr-gcc) で、Elecrow SEA0004T (ATmega328P) を C 言語で開発するためのテンプレートです。
Elecrow 公式の 21 レッスン (Arduino 用サンプル) を、Arduino のライブラリを使わない C 言語に書き直してあります。

## 使い方 (HEX を作るところまで)

1. Microchip AVR 8-bit Toolchain を `C:\avr8-gnu-toolchain-win32_x86_64` に展開する
   (別の場所に置いた場合は `CMakePresets.json` の `AVR_ROOT` を書き換える)
2. Visual Studio で「フォルダーを開く」→ このフォルダーを選ぶ
3. 構成が `SEA0004T (ATmega328P)` になっていることを確認する
4. `lesson_0` をビルドする → `out\build\avr\lesson_0\lesson_0.hex` ができれば成功

## ボードへの書き込み

1. `tools\avrdude\` に `avrdude.exe` と `avrdude.conf` を置く
2. `CMakePresets.json` の `AVR_PORT` は、既定で `ch340` (デバイス名で探す) になっている。
   この指定で見つからないときは、デバイスマネージャーで確認した COM ポート (例: `COM5`) に書き換える
   (デバイス名での指定は、avrdude の mingw 版で動作を確認。`-P ?s` で一覧を表示できる版もある)
   (書き込みに失敗するときは `AVR_BAUD` を `57600` にして試す)
3. ターゲット `upload_lesson_N` (N はレッスン番号) をビルドする

## printf の出力を見る (シリアルモニタ)

1. ターゲット `monitor` をビルドする → 別のコンソールウィンドウが開き、`printf` の出力が表示される
   (`AVR_PORT` と同じ指定でポートを探す。ボードを抜き差ししても、自動でつなぎ直す)
2. 閉じるときは、ウィンドウの `×` か `Ctrl+C`
3. `upload_lesson_N` を実行すると、開いているモニタは**自動で閉じられる** (COM ポートの解放)。
   書き込みのあと、もう一度 `monitor` をビルドすれば、続きを見られる

`monitor` で開いたもの以外 (Arduino IDE のシリアルモニタ、Tera Term など) が COM ポートを開いていると、
書き込みに失敗する。そのときは、それらを閉じる。

## PC とボードが通信するレッスン (comm_*)

`comm_*` フォルダには、ボード側 (`board/`) と PC 側 (`pc/`) のプログラムが入っている。
共通の約束 (命令の文字列) は `protocol.h` に書いてあり、どちらも同じファイルを読む。

1. **ボード側**: 構成を `SEA0004T (ATmega328P)` にして、`upload_comm_1_board` をビルドする (書き込み)
2. **PC 側**: 構成を **`PC (Windows, MSVC)`** に切り替えて、`comm_1_pc` を実行する
   (VS のツールバーで、スタートアップ項目に `comm_1_pc.exe` を選び、`Ctrl+F5`)
3. 実行中の PC プログラムが COM ポートを使っているので、**書き込みの前に、PC プログラムを終了する**

| フォルダ | 内容 |
|---|---|
| `comm_1` | PC から LED を操作する (`LED ON` / `LED OFF`) |
| `comm_2` | PC からセンサの値を読む (`GET POT` → `POT 355`) |

### PC 側の共用ライブラリ (`pc_lib/uart_helper.h`)

```c
UartStream uart;
open_uart_stream_sea0004t(&uart, 115200);   // SEA0004T を自動で探して開く (1=成功, 0=失敗)
uart_wait_line(&uart, "READY", 5000);       // ボードの起動を待つ
fprintf(uart.out, "LED ON\n");              // 送る (fflush は不要)
uart_fgets(line, sizeof line, &uart);       // 1 行受け取る (時間切れなら NULL)
close_uart_stream(&uart);
```

- 読み取り用 (`uart.in`) と書き込み用 (`uart.out`) は、別々の `FILE*`。
- ポートを開くと、ボードが再起動する。必ず `uart_wait_line(..., "READY", ...)` で起動を待ってから送る。
- 受信が 1 秒 (既定) 途切れると、`uart_fgets` は `NULL` を返す。`uart_set_read_timeout` で変えられる。
- USB シリアルの VID/PID は `uart_helper.h` の `SEA0004T_VID` / `SEA0004T_PID`。
  見つからないときは、デバイスマネージャーの「詳細」→「ハードウェア ID」で確かめる。
- ボード側は、`uart_init()` のあと、`printf` で送り、`fgets(buf, n, stdin)` で 1 行受け取れる。
  1 行は改行まで。バッファより長い行は、途中で区切って受け取る。

## 課題用のフォルダを作る (サンプルのコピー)

1. 元にしたいフォルダ (例: `lesson_5`) をコピーして貼り付け、名前を **`work_` で始まる半角英数字**に変える
   (例: `work_kadai1`)。VS のフォルダービューでも、エクスプローラーでもよい。
   貼り付けた直後の名前 (`lesson_5 - コピー` など) は、日本語と空白を含むので、**必ず付け替える**
2. VS が CMake を再構成する (出力ウィンドウに「CMake の生成が完了しました」と出る)
3. ターゲット `work_kadai1` と `upload_work_kadai1` が現れる。`main.c` を編集して、ビルドと書き込みをする

- `CMakeLists.txt` は、**コピーしたままで変更しなくてよい** (中身は `add_avr_lesson_auto()` の 1 行だけ)。
  ターゲット名はフォルダの名前になり、フォルダ内の `.c` ファイルはすべてビルドされる
- フォルダ名は、`lesson_` か `work_` で始めて、半角の英数字と `_` だけにする。
  それ以外の名前や、`CMakeLists.txt` がないフォルダは、警告が出て無視される

## レッスン一覧

| lesson | 内容 | Elecrow |
|---|---|---|
| 0  | 何もしないプログラム (環境確認用) | - |
| 1  | LED の点滅 | L_1 |
| 2  | ボタンで LED | L_2 |
| 3  | 可変抵抗で LED の明るさ (PWM) | L_3 |
| 4  | LCD に文字を表示 | L_4 |
| 5  | 土壌水分センサ | L_5 |
| 6  | 照度センサで街灯 | L_6 |
| 7  | 超音波センサで距離表示 | L_7 |
| 8  | 障害物が近いとリレーと LED | L_8 |
| 9  | 水やりのお知らせ | L_9 |
| 10 | 明るさ表示 | L_10 |
| 11 | 温湿度の表示 | L_11 |
| 12 | サーボモータ | L_12 |
| 13 | リモコンで LED (タイマ割り込み) | L_13 |
| 14 | 天気のお知らせ | L_14 |
| 15 | リモコンでサーボの角度 | L_15 |
| 16 | 自動ドア | L_16 |
| 17 | 人感センサ | L_17 |
| 18 | 音センサでブザー | L_18 |
| 19 | 加速度センサ | L_19 |
| 20 | 廊下の照明 | L_20 |
| 21 | リモコンの電卓 | L_21 |
| comm_1 | PC から LED を操作 (PC ↔ ボード通信) | - |
| comm_2 | PC からセンサの値を読む (PC ↔ ボード通信) | - |

## bsp (ボード用ライブラリ) の構成

| ファイル | 役割 | 使うハード |
|---|---|---|
| `sea0004t.h/.c` | ピン定義、LED・ボタン・リレー・人感センサ | GPIO |
| `sea_time` | `time_millis()` / `time_micros()` / `delay_ms()` | Timer0 |
| `sea_uart` | `printf` で送信、`fgets(..., stdin)` で受信 (115200bps) | UART |
| `sea_adc` | アナログ入力 | ADC |
| `sea_pwm` | LED の明るさ (D10) | Timer1 |
| `sea_servo` | サーボ (D9) | Timer1 |
| `sea_tone` | ブザー (D3) | Timer2 |
| `sea_i2c` | I2C マスタ | TWI |
| `sea_lcd` | 16x2 LCD (I2C 経由) | - |
| `sea_bh1750` / `sea_dht20` / `sea_lsm6ds3` | 照度・温湿度・加速度 | I2C |
| `sea_hcsr04` | 超音波距離センサ | GPIO + Timer0 |
| `sea_ir` | 赤外線リモコン (NEC 方式) | INT0 + Timer0 |

Timer1 は `sea_pwm` と `sea_servo` で共用するため、同じプログラムでは同時に使えません。

## 注意

- ピン割り当ては Elecrow 公式マニュアルとサンプルコードに基づいています。
- 小数は使わず、整数 (10 倍や 100 倍した値) で扱っています。
