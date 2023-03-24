# Remove comment in following line for DEBUG_MODE
# This mode is used also for embedded system
#set(MACHIKANIA_DEBUG_MODE 1)

# Select a configuration from following set statements

# Raspberry Pi Pico + ILI9341 LCD
# YD-RP2040 + ILI9341 LCD
set(MACHIKANIA_BUILD pico_ili9341)

# Raspberry Pi Pico + ILI9488 LCD
# YD-RP2040 + ILI9488 LCD
#set(MACHIKANIA_BUILD pico_ili9488)

# Seeed XIAO RP2040 for embedded development  
# Useful for using UART with XIAO RP2040
#set(MACHIKANIA_BUILD xiao_embed)

# Raspberry Pi Pico W + ILI9341 LCD + Wifi
#set(MACHIKANIA_BUILD pico_w_ili9341)

# Do not touch the lines below

# Force DEBUG_MODE for XIAO board
if (MACHIKANIA_BUILD STREQUAL "xiao_embed")
	set(MACHIKANIA_DEBUG_MODE 1)
endif()

# Select graphic library
if (MACHIKANIA_BUILD STREQUAL "pico_ili9488")
	set(MACHIKANIA_GRAPH_LIB ili9488_spi)
else()
	set(MACHIKANIA_GRAPH_LIB ili9341_spi)
endif()

if (MACHIKANIA_BUILD STREQUAL "pico_w_ili9341")
	set(MACHIKANIA_WIFI withwifi)
	set(PICO_BOARD pico_w)
else()
	set(MACHIKANIA_WIFI withoutwifi)
endif()
