cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

# include configure file for specifying target
include(config.cmake)

project(${MACHIKANIA_CODE_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

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
	
	# Create video library
	add_library(video_files
		interface/${MACHIKANIA_GRAPH_LIB}.c
		interface/${MACHIKANIA_MONITOR_LIB}.c
		interface/fontdata.c
	)
	target_link_libraries(video_files 
		pico_stdlib
		hardware_pwm
		hardware_spi
		hardware_dma	
		hardware_clocks
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
		
	# xxxxx_kb.uf2
	add_executable(${MACHIKANIA_CODE_NAME}_kb
		${MACHIKANIA_EDITOR}.c
		interface/usbkeyboard.c
		interface/hid_virtual_keytable.c
		interface/keyinput.c
		withkeyboard.c
	)
	target_link_libraries(${MACHIKANIA_CODE_NAME}_kb shared_files)
	target_link_libraries(${MACHIKANIA_CODE_NAME}_kb video_files)
	target_link_libraries(${MACHIKANIA_CODE_NAME}_kb wifi_files)
	target_link_libraries(${MACHIKANIA_CODE_NAME}_kb aux_files)
	target_link_libraries(${MACHIKANIA_CODE_NAME}_kb tinyusb_board)
	target_link_libraries(${MACHIKANIA_CODE_NAME}_kb tinyusb_host)
	target_include_directories(${MACHIKANIA_CODE_NAME}_kb PRIVATE ${CMAKE_CURRENT_LIST_DIR}/interface)

	# xxxxx.uf2
	add_executable(${MACHIKANIA_CODE_NAME}
		withoutkeyboard.c
		pcconnect.c
	)
	target_link_libraries(${MACHIKANIA_CODE_NAME} shared_files)
	target_link_libraries(${MACHIKANIA_CODE_NAME} video_files)
	target_link_libraries(${MACHIKANIA_CODE_NAME} wifi_files)
	target_link_libraries(${MACHIKANIA_CODE_NAME} aux_files)

	# enable usb output, disable uart output
	pico_enable_stdio_usb(${MACHIKANIA_CODE_NAME}_kb 0)
	pico_enable_stdio_usb(${MACHIKANIA_CODE_NAME} 1)
	pico_enable_stdio_uart(${MACHIKANIA_CODE_NAME}_kb 0)
	pico_enable_stdio_uart(${MACHIKANIA_CODE_NAME} 0)

	# create map/bin/hex/uf2 file etc.
	pico_add_extra_outputs(${MACHIKANIA_CODE_NAME}_kb)
	pico_add_extra_outputs(${MACHIKANIA_CODE_NAME})

	# use custom linker script
	if(PICO_PLATFORM STREQUAL "rp2350-arm-s")
		pico_set_linker_script(${MACHIKANIA_CODE_NAME}_kb ${CMAKE_CURRENT_SOURCE_DIR}/memmap_machikania_pico2.ld)
		pico_set_linker_script(${MACHIKANIA_CODE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/memmap_machikania_pico2.ld)
	else()
		pico_set_linker_script(${MACHIKANIA_CODE_NAME}_kb ${CMAKE_CURRENT_SOURCE_DIR}/memmap_machikania.ld)
		pico_set_linker_script(${MACHIKANIA_CODE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/memmap_machikania.ld)
	endif()
	
	# set compile definitions
	add_compile_definitions(MACHIKANIA_CONFIG="./config/${MACHIKANIA_BUILD}.h")
	if (MACHIKANIA_DEBUG_MODE)
		add_compile_definitions(MACHIKANIA_DEBUG_MODE)
	endif()
	if (MACHIKANIA_WIFI STREQUAL "withwifi")
		add_compile_definitions(PICO_STACK_SIZE=4096)
		add_compile_definitions(PICO_HEAP_SIZE=4096)
		#add_compile_definitions(machikania_dummy=6)
	endif()
	if (PICO_PLATFORM STREQUAL "rp2350-arm-s")
		add_compile_definitions(PICO_PLATFORM_RP2350)
	endif()
	if (MACHIKANIA_CODE_NAME STREQUAL "phyllosoma")
		add_compile_definitions(MACHIKANIA_PHYLLOSOMA)
	elseif (MACHIKANIA_CODE_NAME STREQUAL "puerulus")
		add_compile_definitions(MACHIKANIA_PUERULUS)
	endif()
	
elseif(PICO_ON_DEVICE)
	message(WARNING "not building kmbasic because TinyUSB submodule is not initialized in the SDK")
endif()
