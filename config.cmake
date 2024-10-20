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

# Raspberry Pi Pico + NTSC Video out for Puerulus
# YD-RP2040 + NTSC Video
#set(MACHIKANIA_BUILD pico_ntsc)

# Seeed XIAO RP2040 + NTSC Video out for Puerulus
#set(MACHIKANIA_BUILD xiao_ntsc)

################################
# Do not touch the lines below #
################################

# Force DEBUG_MODE for XIAO board
if (MACHIKANIA_BUILD STREQUAL "xiao_embed")
	set(MACHIKANIA_DEBUG_MODE 1)
endif()

# Select graphic library
if (MACHIKANIA_BUILD STREQUAL "pico_ili9488")
	set(MACHIKANIA_GRAPH_LIB ili9488_spi)
elseif (MACHIKANIA_BUILD STREQUAL "pico_ntsc")
	set(MACHIKANIA_GRAPH_LIB rp2040_pwm_ntsc_textgraph)
elseif (MACHIKANIA_BUILD STREQUAL "xiao_ntsc")
	set(MACHIKANIA_GRAPH_LIB rp2040_pwm_ntsc_textgraph)
else()
	set(MACHIKANIA_GRAPH_LIB ili9341_spi)
endif()

# Select monitor library
if (MACHIKANIA_BUILD STREQUAL "pico_ntsc")
	set(MACHIKANIA_MONITOR_LIB graphlib_ntsc)
	set(MACHIKANIA_EDITOR editor_ntsc)
	set(MACHIKANIA_CODE_NAME puerulus)
elseif (MACHIKANIA_BUILD STREQUAL "xiao_ntsc")
	set(MACHIKANIA_MONITOR_LIB graphlib_ntsc)
	set(MACHIKANIA_EDITOR editor_ntsc)
	set(MACHIKANIA_CODE_NAME puerulus)
else()
	set(MACHIKANIA_MONITOR_LIB graphlib_lcd)
	set(MACHIKANIA_EDITOR editor)
	set(MACHIKANIA_CODE_NAME phyllosoma)
endif()

if (PICO_BOARD STREQUAL "pico_w")
	set(MACHIKANIA_WIFI withwifi)
	if (MACHIKANIA_BUILD STREQUAL "pico_ntsc")
		set(MACHIKANIA_BUILD pico_w_ntsc)
	elseif (MACHIKANIA_BUILD STREQUAL "pico_ili9488")
		set(MACHIKANIA_BUILD pico_w_ili9488)
	else()
		set(MACHIKANIA_BUILD pico_w_ili9341)
	endif()
else()
	set(MACHIKANIA_WIFI withoutwifi)
endif()
