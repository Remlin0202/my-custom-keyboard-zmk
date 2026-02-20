#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zmk/events/hid_indicators_changed.h>

#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)

#define LED_NODE DT_ALIAS(led_caps)

#if DT_NODE_HAS_STATUS(LED_NODE, okay)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

static int caps_led_init(void) {
    if (!gpio_is_ready_dt(&led)) {
        return -ENODEV;
    }
    return gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
}

SYS_INIT(caps_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static int caps_led_listener_cb(const zmk_event_t *eh) {
    const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);
    
    // USB HID standard: Num Lock is bit 0, Caps Lock is bit 1, Scroll Lock is bit 2
    if (!ev) {
        return 0;
    }

    if (ev->indicators & BIT(1)) { 
        gpio_pin_set_dt(&led, 1);
    } else {
        gpio_pin_set_dt(&led, 0);
    }
    return 0;
}

ZMK_LISTENER(caps_led_listener, caps_led_listener_cb);
ZMK_SUBSCRIPTION(caps_led_listener, zmk_hid_indicators_changed);

#endif // DT_NODE_HAS_STATUS
#endif // IS_ENABLED
