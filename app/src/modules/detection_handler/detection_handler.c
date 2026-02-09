/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <zephyr/zbus/zbus.h>
#include <stdlib.h>

#include "../../../../../opt-modules/edgeAI/detection.h"

LOG_MODULE_REGISTER(detection_handler, CONFIG_APP_DETECTION_HANDLER_LOG_LEVEL);

/* Statistics */
static uint32_t detection_count = 0;
static bool service_running = false;

/**
 * @brief ZBus listener callback for detection events
 * Monitors all detection module events and logs them
 */
static void detection_listener_cb(const struct zbus_channel *chan)
{
	const struct detection_msg *msg = zbus_chan_const_msg(chan);

	switch (msg->type) {
	case DETECTION_STARTED:
		LOG_INF("=== Detection service STARTED ===");
		service_running = true;
		detection_count = 0;
		break;

	case DETECTION_STOPPED:
		LOG_INF("=== Detection service STOPPED ===");
		LOG_INF("Total detections: %u", detection_count);
		service_running = false;
		break;

	case DETECTION_RESULT:
		detection_count++;
		LOG_INF("Detection #%u: %s (class %u), confidence=%.1f%%",
			detection_count,
			detection_class_name(msg->result.predicted_class),
			msg->result.predicted_class,
			(double)(msg->result.confidence * 100.0f));
		break;

	case DETECTION_START:
	case DETECTION_STOP:
		/* Ignore command messages - we only care about events */
		break;

	default:
		LOG_WRN("Unknown detection message type: %d", msg->type);
		break;
	}
}

ZBUS_LISTENER_DEFINE(detection_handler_listener, detection_listener_cb);
ZBUS_CHAN_ADD_OBS(DETECTION_CHAN, detection_handler_listener, 0);

/**
 * @brief Shell command: start detection service
 */
static int cmd_detection_start(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	struct detection_msg msg = {
		.type = DETECTION_START,
		.timestamp = k_uptime_get()
	};

	int ret = zbus_chan_pub(&DETECTION_CHAN, &msg, K_NO_WAIT);
	if (ret) {
		LOG_ERR("Failed to publish START: %d", ret);
		shell_print(sh, "Failed to send START command");
		return ret;
	}

	shell_print(sh, "Detection START command sent");
	return 0;
}

/**
 * @brief Shell command: stop detection service
 */
static int cmd_detection_stop(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	struct detection_msg msg = {
		.type = DETECTION_STOP,
		.timestamp = k_uptime_get()
	};

	int ret = zbus_chan_pub(&DETECTION_CHAN, &msg, K_NO_WAIT);
	if (ret) {
		LOG_ERR("Failed to publish STOP: %d", ret);
		shell_print(sh, "Failed to send STOP command");
		return ret;
	}

	shell_print(sh, "Detection STOP command sent");
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(detection_handler_cmds,
	SHELL_CMD(start, NULL, "Start detection service", cmd_detection_start),
	SHELL_CMD(stop, NULL, "Stop detection service", cmd_detection_stop),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(detection_handler, &detection_handler_cmds, "Detection handler module commands", NULL);
