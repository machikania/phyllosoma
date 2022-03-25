cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(lcd-lib C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

if (TARGET tinyusb_device)
	add_executable(phyllosoma
			main.c
			api.c
			library.c
			compiler.c
			run.c
			statements.c
			functions.c
			integer.c
			float.c
			string.c
			globalvars.c
			variable.c
			operators.c
			value.c
			cmpdata.c
			error.c
			memory.c
			class.c
			file.c
			display.c
			timer.c
			debug.c
			interface/fontdata.c
			interface/graphlib.c
			interface/ili9341_spi.c
			interface/ff.c
			interface/pico-sdmm.c
			interface/fileselect.c
		)

	# Pull in our pico_stdlib which aggregates commonly used features
	target_link_libraries(phyllosoma pico_stdlib hardware_spi)

	# enable usb output, disable uart output
	pico_enable_stdio_usb(phyllosoma 1)
	pico_enable_stdio_uart(phyllosoma 0)

	# create map/bin/hex/uf2 file etc.
	pico_add_extra_outputs(phyllosoma)

elseif(PICO_ON_DEVICE)
	message(WARNING "not building kmbasic because TinyUSB submodule is not initialized in the SDK")
endif()
