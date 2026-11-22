# PC 側の共用 UART ライブラリ。通信レッスンから自動でリンクされる
add_library(sea_pc_uart STATIC uart_helper.c "win_msgbox.c" "win_msgbox.h" "input_box.c" "gui_app.c"  )
target_include_directories(sea_pc_uart PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sea_pc_uart PUBLIC setupapi)
