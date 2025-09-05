# Add segger tracing
target_sources(app PRIVATE
	${ZEPHYR_BASE}/../modules/debug/segger/SEGGER/SEGGER_RTT_printf.c
	zbus_tracing.c
	)

target_include_directories(app PRIVATE .)