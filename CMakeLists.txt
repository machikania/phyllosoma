if (TARGET tinyusb_device)
	add_executable(kmbasic
			kmbasic.c
			api.c
			library.c
			compiler.c
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
		)

	# Pull in our pico_stdlib which aggregates commonly used features
	target_link_libraries(kmbasic pico_stdlib)

	# enable usb output, disable uart output
	pico_enable_stdio_usb(kmbasic 1)
	pico_enable_stdio_uart(kmbasic 0)

	# create map/bin/hex/uf2 file etc.
	pico_add_extra_outputs(kmbasic)

	# add url via pico_set_program_url
	example_auto_set_url(kmbasic)
elseif(PICO_ON_DEVICE)
	message(WARNING "not building kmbasic because TinyUSB submodule is not initialized in the SDK")
endif()
