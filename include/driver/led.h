/**
 * @file AVR32DB28 LED driver.
 */
#ifndef LED_H_
#define LED_H_

#include <stdbool.h>
#include <stdint.h>

/** LED IDs. */
#define LED1 0U // LED1.
#define LED2 1U // LED2.
#define LED3 2U // LED3.

/**
 * @brief Initialize all LEDs.
 */
void led_setup(void);

/**
 * @brief Get LED state.
 *
 * @param[in] led LED ID.
 *
 * @return True if the LED is on, false otherwise.
 */
bool led_read(uint8_t led);

/**
 * @brief Set LED state.
 *
 * @param[in] led LED ID.
 * @param[in] state LED state (true = on, false = off).
 */
void led_write(uint8_t led, bool state);

/**
 * @brief Toggle LED.
 *
 * @param[in] led LED ID.
 */
void led_toggle(uint8_t led);

/**
 * @brief Run PWM on LED.
 *
 * @param[in] led LED ID.
 * @param[in] percent Duty cycle in percent, from 0 (off) to 100 (fully on).
 *
 * @note The duty cycle is rounded to the nearest ten percent, giving ten brightness steps.
 *       A duty cycle above 100 is ignored.
 */
void led_pwm(uint8_t led, uint8_t percent);

#endif /** LED_H_ */
