#if defined(CONFIG_TRACE_ZBUS)

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include "zbus_tracing.h"

#if defined(CONFIG_TRACE_ZBUS_BUTTON)
#include "../button/button.h"
#endif
#if defined(CONFIG_TRACE_ZBUS_CLOUD)
#include "../cloud/cloud.h"
#endif
#if defined(CONFIG_TRACE_ZBUS_ENVIRONMENTAL)
#include "../environmental/environmental.h"
#endif
#if defined(CONFIG_TRACE_ZBUS_FOTA)
#include "../fota/fota.h"
#endif
#if defined(CONFIG_TRACE_ZBUS_LED)
#include "../led/led.h"
#endif
#if defined(CONFIG_TRACE_ZBUS_LOCATION)
#include "../location/location.h"
#endif
#if defined(CONFIG_TRACE_ZBUS_NETWORK)
#include "../network/network.h"
#endif
#if defined(CONFIG_TRACE_ZBUS_POWER)
#include "../power/power.h"
#endif

typedef void (*channel_decoder_t)(const void *msg_data);

struct channel_handler
{
	const struct zbus_channel *chan;
	channel_decoder_t decoder;
};

static void auto_trace_callback(const struct zbus_channel *chan);
ZBUS_LISTENER_DEFINE(auto_trace_listener, auto_trace_callback);

#if defined(CONFIG_TRACE_ZBUS_BUTTON)
static void button_msg_decode(const void *msg_data)
{
	const struct button_msg *msg = (const struct button_msg *)msg_data;

	TRACE_ZBUS("BUTTON_CHANNEL", msg->type == BUTTON_PRESS_SHORT ? "SHORT_PRESS" : "LONG_PRESS");
}
ZBUS_CHAN_ADD_OBS(BUTTON_CHAN, auto_trace_listener, 0);
#endif

#if defined(CONFIG_TRACE_ZBUS_CLOUD)
static void cloud_msg_decode(const void *msg_data)
{
	const struct cloud_msg *cloud = (const struct cloud_msg *)msg_data;

	const char *type_str;
	switch (cloud->type)
	{
	case CLOUD_DISCONNECTED:
		type_str = "DISCONNECTED";
		break;
	case CLOUD_CONNECTED:
		type_str = "CONNECTED";
		break;
	case CLOUD_CONNECTION_ATTEMPT_COUNT_REACHED:
		type_str = "CONN_ATTEMPTS_REACHED";
		break;
	case CLOUD_PAYLOAD_JSON:
		type_str = "PAYLOAD_JSON";
		break;
	case CLOUD_POLL_SHADOW:
		type_str = "POLL_SHADOW";
		break;
	case CLOUD_SHADOW_RESPONSE_DESIRED:
		type_str = "SHADOW_RESPONSE_DESIRED";
		break;
	case CLOUD_SHADOW_RESPONSE_DELTA:
		type_str = "SHADOW_RESPONSE_DELTA";
		break;
	default:
		type_str = "UNKNOWN";
		break;
	}

	TRACE_ZBUS("CLOUD_CHAN", type_str);
}
ZBUS_CHAN_ADD_OBS(CLOUD_CHAN, auto_trace_listener, 0);
#endif

#if defined(CONFIG_TRACE_ZBUS_ENVIRONMENTAL)
static void environmental_msg_decode(const void *msg_data)
{
	const struct environmental_msg *msg = (const struct environmental_msg *)msg_data;

	TRACE_ZBUS("ENVIRONMENTAL_CHAN", msg->type == ENVIRONMENTAL_SENSOR_SAMPLE_REQUEST ? "SENSOR_SAMPLE_REQUEST" : "SENSOR_SAMPLE_RESPONSE");
}
ZBUS_CHAN_ADD_OBS(ENVIRONMENTAL_CHAN, auto_trace_listener, 0);
#endif

#if defined(CONFIG_TRACE_ZBUS_FOTA)
static void fota_msg_decode(const void *msg_data)
{
	const enum fota_msg_type *msg = (const enum fota_msg_type *)msg_data;

	const char *type_str;
	switch (*msg)
	{
	case FOTA_DOWNLOAD_FAILED:
		type_str = "DOWNLOAD_FAILED";
		break;
	case FOTA_DOWNLOAD_TIMED_OUT:
		type_str = "DOWNLOAD_TIMED_OUT";
		break;
	case FOTA_DOWNLOADING_UPDATE:
		type_str = "DOWNLOADING_UPDATE";
		break;
	case FOTA_NO_AVAILABLE_UPDATE:
		type_str = "NO_AVAILABLE_UPDATE";
		break;
	case FOTA_SUCCESS_REBOOT_NEEDED:
		type_str = "SUCCESS_REBOOT_NEEDED";
		break;
	case FOTA_IMAGE_APPLY_NEEDED:
		type_str = "IMAGE_APPLY_NEEDED";
		break;
	case FOTA_DOWNLOAD_CANCELED:
		type_str = "DOWNLOAD_CANCELED";
		break;
	case FOTA_POLL_REQUEST:
		type_str = "POLL_REQUEST";
		break;
	case FOTA_IMAGE_APPLY:
		type_str = "IMAGE_APPLY";
		break;
	case FOTA_DOWNLOAD_CANCEL:
		type_str = "DOWNLOAD_CANCEL";
		break;
	default:
		type_str = "UNKNOWN";
		break;
	}

	TRACE_ZBUS("FOTA_CHAN", type_str);
}
ZBUS_CHAN_ADD_OBS(FOTA_CHAN, auto_trace_listener, 0);
#endif

#if defined(CONFIG_TRACE_ZBUS_LED)
static void led_msg_decode(const void *msg_data)
{
	TRACE_ZBUS("LED_CHAN", "LED_UPDATE");
}
ZBUS_CHAN_ADD_OBS(LED_CHAN, auto_trace_listener, 0);
#endif

#if defined(CONFIG_TRACE_ZBUS_LOCATION)
static void location_msg_decode(const void *msg_data)
{
	const struct location_msg *loc = (const struct location_msg *)msg_data;

	switch (loc->type)
	{
	case LOCATION_SEARCH_STARTED:
		TRACE_ZBUS("LOCATION_CHAN", "SEARCH_STARTED");
		break;
	case LOCATION_SEARCH_DONE:
		TRACE_ZBUS("LOCATION_CHAN", "SEARCH_DONE");
		break;
	case LOCATION_SEARCH_TRIGGER:
		TRACE_ZBUS("LOCATION_CHAN", "SEARCH_TRIGGER");
		break;
	case LOCATION_CLOUD_REQUEST:
		TRACE_ZBUS("LOCATION_CHAN", "CLOUD_REQUEST");
		break;
	case LOCATION_AGNSS_REQUEST:
		TRACE_ZBUS("LOCATION_CHAN", "AGNSS_REQUEST");
		break;
	case LOCATION_GNSS_DATA:
		TRACE_ZBUS("LOCATION_CHAN", "GNSS_DATA");
		break;
	default:
		TRACE_ZBUS("LOCATION_CHAN", "UNKNOWN");
		break;
	}
}
ZBUS_CHAN_ADD_OBS(LOCATION_CHAN, auto_trace_listener, 0);
#endif

#if defined(CONFIG_TRACE_ZBUS_NETWORK)
static void network_msg_decode(const void *msg_data)
{
	const struct network_msg *net = (const struct network_msg *)msg_data;

	switch (net->type)
	{
	case NETWORK_CONNECTED:
		TRACE_ZBUS("NETWORK_CHAN", "CONNECTED");
		break;
	case NETWORK_DISCONNECTED:
		TRACE_ZBUS("NETWORK_CHAN", "DISCONNECTED");
		break;
	case NETWORK_CONNECT:
		TRACE_ZBUS("NETWORK_CHAN", "CONNECT_REQ");
		break;
	case NETWORK_DISCONNECT:
		TRACE_ZBUS("NETWORK_CHAN", "DISCONNECT_REQ");
		break;
	case NETWORK_ATTACH_REJECTED:
		TRACE_ZBUS("NETWORK_CHAN", "ATTACH_REJECTED");
		break;
	case NETWORK_MODEM_RESET_LOOP:
		TRACE_ZBUS("NETWORK_CHAN", "RESET_LOOP");
		break;
	case NETWORK_UICC_FAILURE:
		TRACE_ZBUS("NETWORK_CHAN", "SIM_FAILURE");
		break;
	default:
		TRACE_ZBUS("NETWORK_CHAN", "OTHER");
		break;
	}
}
ZBUS_CHAN_ADD_OBS(NETWORK_CHAN, auto_trace_listener, 0);
#endif

#if defined(CONFIG_TRACE_ZBUS_POWER)
static void power_msg_decode(const void *msg_data)
{
	const struct power_msg *power = (const struct power_msg *)msg_data;

	TRACE_ZBUS("POWER_CHAN",
		   power->type == POWER_BATTERY_PERCENTAGE_SAMPLE_REQUEST ? "SAMPLE_REQUEST" : "SAMPLE_RESPONSE");
}
ZBUS_CHAN_ADD_OBS(POWER_CHAN, auto_trace_listener, 0);
#endif

static const struct channel_handler channel_handlers[] = {
#if defined(CONFIG_TRACE_ZBUS_BUTTON)
    {.chan = &BUTTON_CHAN, .decoder = button_msg_decode},
#endif
#if defined(CONFIG_TRACE_ZBUS_CLOUD)
    {.chan = &CLOUD_CHAN, .decoder = cloud_msg_decode},
#endif
#if defined(CONFIG_TRACE_ZBUS_ENVIRONMENTAL)
    {.chan = &ENVIRONMENTAL_CHAN, .decoder = environmental_msg_decode},
#endif
#if defined(CONFIG_TRACE_ZBUS_FOTA)
    {.chan = &FOTA_CHAN, .decoder = fota_msg_decode},
#endif
#if defined(CONFIG_TRACE_ZBUS_LED)
    {.chan = &LED_CHAN, .decoder = led_msg_decode},
#endif
#if defined(CONFIG_TRACE_ZBUS_LOCATION)
    {.chan = &LOCATION_CHAN, .decoder = location_msg_decode},
#endif
#if defined(CONFIG_TRACE_ZBUS_NETWORK)
    {.chan = &NETWORK_CHAN, .decoder = network_msg_decode},
#endif
#if defined(CONFIG_TRACE_ZBUS_POWER)
    {.chan = &POWER_CHAN, .decoder = power_msg_decode},
#endif
};

static void auto_trace_callback(const struct zbus_channel *chan)
{
	const void *msg_data = zbus_chan_const_msg(chan);

	// Find and call the appropriate decoder
	for (int i = 0; i < ARRAY_SIZE(channel_handlers); i++)
	{
		if (chan == channel_handlers[i].chan)
		{
			channel_handlers[i].decoder(msg_data);
			return;
		}
	}

	// Fallback for unhandled channels
	const char *channel_name = zbus_chan_name(chan);
	TRACE_ZBUS(channel_name ? channel_name : "unknown", "unhandled");
}

#endif /* CONFIG_TRACE_ZBUS */