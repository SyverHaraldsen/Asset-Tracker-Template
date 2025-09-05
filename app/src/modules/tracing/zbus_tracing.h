#ifndef ZBUS_TRACING_H
#define ZBUS_TRACING_H

#if defined(CONFIG_TRACE_ZBUS)
#include <SEGGER_SYSVIEW.h>
#define TRACE_ZBUS(channel, value) \
	SEGGER_SYSVIEW_PrintfTarget("zbus/%s, %s", channel, value)
#else
#define TRACE_ZBUS(channel, value)
#endif

#endif /* ZBUS_TRACING_H */