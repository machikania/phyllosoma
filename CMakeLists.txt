cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(lcd-lib C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

if (TARGET tinyusb_device)
	add_executable(phyllosoma_kb
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
		io.c
		music.c
		core1.c
		hexfile.c
		debug.c
		editor.c
		interface/fontdata.c
		interface/graphlib.c
		interface/ili9341_spi.c
		interface/ff.c
		interface/pico-sdmm.c
		interface/fileselect.c
		interface/usbkeyboard.c
		interface/hid_virtual_keytable.c
		interface/keyinput.c
		withkeyboard.c
	)
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
		io.c
		music.c
		core1.c
		hexfile.c
		debug.c
		interface/fontdata.c
		interface/graphlib.c
		interface/ili9341_spi.c
		interface/ff.c
		interface/pico-sdmm.c
		interface/fileselect.c
		withoutkeyboard.c
		pcconnect.c
	)
	# Pull in our pico_stdlib which aggregates commonly used features
	target_link_libraries(phyllosoma_kb pico_stdlib)
	target_link_libraries(phyllosoma pico_stdlib)
	target_link_libraries(phyllosoma_kb pico_multicore)
	target_link_libraries(phyllosoma pico_multicore)
	target_link_libraries(phyllosoma_kb hardware_spi)
	target_link_libraries(phyllosoma hardware_spi)
	target_link_libraries(phyllosoma_kb hardware_pwm)
	target_link_libraries(phyllosoma hardware_pwm)
	target_link_libraries(phyllosoma_kb hardware_adc)
	target_link_libraries(phyllosoma hardware_adc)
	target_link_libraries(phyllosoma_kb hardware_i2c)
	target_link_libraries(phyllosoma hardware_i2c)
	target_link_libraries(phyllosoma_kb tinyusb_board)
	target_link_libraries(phyllosoma_kb tinyusb_host)
	target_include_directories(phyllosoma_kb PRIVATE ${CMAKE_CURRENT_LIST_DIR}/interface)

	# enable usb output, disable uart output
	pico_enable_stdio_usb(phyllosoma_kb 0)
	pico_enable_stdio_usb(phyllosoma 1)
	pico_enable_stdio_uart(phyllosoma_kb 0)
	pico_enable_stdio_uart(phyllosoma 0)

	# create map/bin/hex/uf2 file etc.
	pico_add_extra_outputs(phyllosoma_kb)
	pico_add_extra_outputs(phyllosoma)

	# use custom linker script
	pico_set_linker_script(phyllosoma_kb ${CMAKE_CURRENT_SOURCE_DIR}/memmap_machikania.ld)
	pico_set_linker_script(phyllosoma ${CMAKE_CURRENT_SOURCE_DIR}/memmap_machikania.ld)

elseif(PICO_ON_DEVICE)
	message(WARNING "not building kmbasic because TinyUSB submodule is not initialized in the SDK")
endif()
