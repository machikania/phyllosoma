cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(lcd-lib C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# include configure file for specifying target
include(config.cmake)

# Initialize the SDK
pico_sdk_init()

if (TARGET tinyusb_device)
	# Create library including shared files
	add_library(shared_files
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
		exception.c
		rtc.c
		interface/ff.c
		interface/pico-sdmm.c
		interface/fileselect.c
	)
	target_link_libraries(shared_files 
		pico_stdlib
		pico_multicore
		hardware_spi
		hardware_pwm
		hardware_adc
		hardware_i2c
		hardware_exception
		pico_aon_timer
	)
	
	# Create graphic library
	add_library(graphic_files
		interface/${MACHIKANIA_GRAPH_LIB}.c
		interface/fontdata.c
		interface/graphlib.c	
	)
	target_link_libraries(graphic_files 
		pico_stdlib
		hardware_spi
	)
	
	# Create wifi library
	if (MACHIKANIA_WIFI STREQUAL "withwifi")
		add_library(wifi_files
			wifi/withwifi.c
			wifi/wifierror.c
			wifi/socket.c
			wifi/picow_ntp_client.c
			wifi/picow_tcp_client.c
			wifi/picow_tcp_server.c
			wifi/picow_tls_client.c
		)
		target_link_libraries(wifi_files
			pico_cyw43_arch_lwip_threadsafe_background
			pico_lwip_mbedtls
			pico_mbedtls
		)
		target_include_directories(wifi_files PUBLIC
			${CMAKE_CURRENT_LIST_DIR}
			${CMAKE_CURRENT_LIST_DIR}/wifi
		)
		target_link_libraries(wifi_files 
			pico_stdlib
		)
	else()
		add_library(wifi_files
			wifi/withoutwifi.c
		)
		target_link_libraries(wifi_files 
			pico_stdlib
		)
	endif()
	
	# Create Auxiliary code library (include file in subdirectory)
	include(auxcode/auxcode.cmake)
	# The default definition is as follows
		# add_library(aux_files
		#	auxcode/auxcode.c
		# )
		# target_link_libraries(aux_files 
		#	pico_stdlib
		# )
		
	# phyllosoma_kb.uf2
	add_executable(phyllosoma_kb
		editor.c
		interface/usbkeyboard.c
		interface/hid_virtual_keytable.c
		interface/keyinput.c
		withkeyboard.c
	)
	target_link_libraries(phyllosoma_kb shared_files)
	target_link_libraries(phyllosoma_kb graphic_files)
	target_link_libraries(phyllosoma_kb wifi_files)
	target_link_libraries(phyllosoma_kb aux_files)
	target_link_libraries(phyllosoma_kb tinyusb_board)
	target_link_libraries(phyllosoma_kb tinyusb_host)
	target_include_directories(phyllosoma_kb PRIVATE ${CMAKE_CURRENT_LIST_DIR}/interface)

	# phyllosoma.uf2
	add_executable(phyllosoma
		withoutkeyboard.c
		pcconnect.c
	)
	target_link_libraries(phyllosoma shared_files)
	target_link_libraries(phyllosoma graphic_files)
	target_link_libraries(phyllosoma wifi_files)
	target_link_libraries(phyllosoma aux_files)

	# enable usb output, disable uart output
	pico_enable_stdio_usb(phyllosoma_kb 0)
	pico_enable_stdio_usb(phyllosoma 1)
	pico_enable_stdio_uart(phyllosoma_kb 0)
	pico_enable_stdio_uart(phyllosoma 0)

	# create map/bin/hex/uf2 file etc.
	pico_add_extra_outputs(phyllosoma_kb)
	pico_add_extra_outputs(phyllosoma)

	# use custom linker script
	if(PICO_PLATFORM STREQUAL "rp2350-arm-s")
		pico_set_linker_script(phyllosoma_kb ${CMAKE_CURRENT_SOURCE_DIR}/memmap_machikania_pico2.ld)
		pico_set_linker_script(phyllosoma ${CMAKE_CURRENT_SOURCE_DIR}/memmap_machikania_pico2.ld)
	else()
		pico_set_linker_script(phyllosoma_kb ${CMAKE_CURRENT_SOURCE_DIR}/memmap_machikania.ld)
		pico_set_linker_script(phyllosoma ${CMAKE_CURRENT_SOURCE_DIR}/memmap_machikania.ld)
	endif()
	
	# set compile definitions
	add_compile_definitions(MACHIKANIA_CONFIG="./config/${MACHIKANIA_BUILD}.h")
	if (MACHIKANIA_DEBUG_MODE)
		add_compile_definitions(MACHIKANIA_DEBUG_MODE)
	endif()
	if (MACHIKANIA_WIFI STREQUAL "withwifi")
		add_compile_definitions(PICO_STACK_SIZE=4096)
		add_compile_definitions(PICO_HEAP_SIZE=4096)
		#target_compile_definitions(wifi_files PRIVATE machikania_dummy=6)
	endif()
	if(PICO_PLATFORM STREQUAL "rp2350-arm-s")
		add_compile_definitions(PICO_PLATFORM_RP2350)
	endif()
	
elseif(PICO_ON_DEVICE)
	message(WARNING "not building kmbasic because TinyUSB submodule is not initialized in the SDK")
endif()
