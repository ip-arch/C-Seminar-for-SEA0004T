# PC 側の共用 UART ライブラリ。通信レッスンから自動でリンクされる
add_library(sea_pc_uart STATIC uart_helper.c)
target_include_directories(sea_pc_uart PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sea_pc_uart PUBLIC setupapi)
