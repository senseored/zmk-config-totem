/*
 *
 * Copyright (c) 2024 Tobias Gustafsson
 * SPDX-License-Identifier: MIT
 *
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/hid_indicators.h>
#include <zmk/hid.h>
#include <zmk/keymap.h>

bool layer_name_start(const char *a, const char *b) {
    if (strncmp(a, b, strlen(b)) == 0)
        return 1;
    return 0;
}

int current_layer_status_state(const zmk_event_t *eh) {
    int err;
    uint8_t index = zmk_keymap_highest_layer_active();
    const char *layer_name = zmk_keymap_layer_name(index);

    const zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();

    LOG_DBG("Active layer: %i, layer name: %s, led indicators: %x", index, layer_name, indicators);
    if ((layer_name_start(layer_name, "NUML_")) &&
        (indicators % 2 == 0)) { // layers that should have numlock on
        LOG_DBG("numlock not active - turn on");
        // SEND NUMLOCK
        err = zmk_hid_press(ZMK_HID_USAGE(0x07, 0x53));
        if (err < 0) {
            LOG_DBG("Unable to press numlock");
            return err;
        } else {
            err = zmk_endpoints_send_report(HID_USAGE_KEY);
        }
        err = zmk_hid_release(ZMK_HID_USAGE(0x07, 0x53));
        if (err < 0) {
            LOG_DBG("Unable to release numlock");
            return err;
        } else {
            err = zmk_endpoints_send_report(HID_USAGE_KEY);
        }
    } else if ((index == 0) && (indicators % 2 != 0)) { // layers that should have numlock off
        LOG_DBG("numlock active - turn off");
        // SEND NUMLOCK
        err = zmk_hid_press(ZMK_HID_USAGE(0x07, 0x53));
        if (err < 0) {
            LOG_DBG("Unable to press numlock");
            return err;
        } else {
            err = zmk_endpoints_send_report(HID_USAGE_KEY);
        }
        err = zmk_hid_release(ZMK_HID_USAGE(0x07, 0x53));
        if (err < 0) {
            LOG_DBG("Unable to release numlock");
            return err;
        } else {
            err = zmk_endpoints_send_report(HID_USAGE_KEY);
        }
    }
    return 0;
}

ZMK_LISTENER(current_layer_status_state, current_layer_status_state);
ZMK_SUBSCRIPTION(current_layer_status_state, zmk_layer_state_changed);
