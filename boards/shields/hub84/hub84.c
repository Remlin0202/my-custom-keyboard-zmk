#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/sys/util.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/battery.h>

/* --- Caps Lock LED Logic (GPIO) --- */
#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
#define LED_CAPS_NODE DT_ALIAS(led_caps)
#if DT_NODE_HAS_STATUS(LED_CAPS_NODE, okay)

static const struct gpio_dt_spec led_caps = GPIO_DT_SPEC_GET(LED_CAPS_NODE, gpios);

static int caps_led_init(void) {
    if (!gpio_is_ready_dt(&led_caps)) {
        return -ENODEV;
    }
    return gpio_pin_configure_dt(&led_caps, GPIO_OUTPUT_INACTIVE);
}
SYS_INIT(caps_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static int caps_led_listener_cb(const zmk_event_t *eh) {
    const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);
    if (!ev) return 0;

    if (ev->indicators & BIT(1)) { 
        gpio_pin_set_dt(&led_caps, 1);
    } else {
        gpio_pin_set_dt(&led_caps, 0);
    }
    return 0;
}
ZMK_LISTENER(caps_led_listener, caps_led_listener_cb);
ZMK_SUBSCRIPTION(caps_led_listener, zmk_hid_indicators_changed);

#endif
#endif

/* --- Low Battery Breathing LED Logic (PWM) --- */
#define LED_BAT_NODE DT_ALIAS(led_bat)
#if DT_NODE_HAS_STATUS(LED_BAT_NODE, okay)

static const struct pwm_dt_spec led_bat = PWM_DT_SPEC_GET(LED_BAT_NODE);

#define BREATHE_STEPS 50
#define BREATHE_STEP_MS 20
static uint32_t current_step = 0;
static bool breathing_up = true;
static bool is_breathing_active = false;

static void breathe_timer_handler(struct k_timer *timer_id) {
    if (!is_breathing_active) return;
    
    uint32_t pulse = (led_bat.period / BREATHE_STEPS) * current_step;
    pwm_set_dt(&led_bat, led_bat.period, pulse);

    if (breathing_up) {
        current_step++;
        if (current_step >= BREATHE_STEPS) breathing_up = false;
    } else {
        if (current_step > 0) current_step--;
        if (current_step == 0) breathing_up = true;
    }
}

K_TIMER_DEFINE(breathe_timer, breathe_timer_handler, NULL);

static int bat_led_init(void) {
    if (!pwm_is_ready_dt(&led_bat)) {
        return -ENODEV;
    }
    // Turn off initially
    return pwm_set_dt(&led_bat, led_bat.period, 0);
}
SYS_INIT(bat_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static int bat_led_listener_cb(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);
    if (!ev) return 0;

    // Define low battery threshold (temporarily set to <= 100% for testing)
    if (ev->state_of_charge <= 100) {
        if (!is_breathing_active) {
            is_breathing_active = true;
            k_timer_start(&breathe_timer, K_MSEC(BREATHE_STEP_MS), K_MSEC(BREATHE_STEP_MS));
        }
    } else {
        if (is_breathing_active) {
            is_breathing_active = false;
            k_timer_stop(&breathe_timer);
            pwm_set_dt(&led_bat, led_bat.period, 0); // Turn off LED completely
        }
    }
    return 0;
}
ZMK_LISTENER(bat_led_listener, bat_led_listener_cb);
ZMK_SUBSCRIPTION(bat_led_listener, zmk_battery_state_changed);

#endif
