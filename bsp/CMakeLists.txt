add_library(sea0004t_bsp STATIC
  sea0004t.c
  sea_time.c
  sea_uart.c
  sea_adc.c
  sea_pwm.c
  sea_tone.c
  sea_servo.c
  sea_i2c.c
  sea_lcd.c
  sea_bh1750.c
  sea_dht20.c
  sea_lsm6ds3.c
  sea_hcsr04.c
  sea_ir.c)
target_include_directories(sea0004t_bsp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sea0004t_bsp PUBLIC avr_common)
