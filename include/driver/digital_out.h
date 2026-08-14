/**
 * @file AVR32DB28 digital output driver.
 */
#ifndef DIGITAL_OUT_H_
#define DIGITAL_OUT_H_

#include <stdbool.h>
#include <stdint.h>

/** Digital output pins. */
#define PC0 0U  // PORTC pin 0.
#define PC1 1U  // PORTC pin 1.
#define PC2 2U  // PORTC pin 2.
#define PC3 3U  // PORTC pin 3.
#define PD1 4U  // PORTD pin 1.
#define PD2 5U  // PORTD pin 2.
#define PD3 6U  // PORTD pin 3.
#define PD4 7U  // PORTD pin 4.
#define PD5 8U  // PORTD pin 5.
#define PD6 9U  // PORTD pin 6.
#define PD7 10U // PORTD pin 7.
#define PF0 11U // PORTF pin 0.
#define PF1 12U // PORTF pin 1.
#define PA0 13U // PORTA pin 0.
#define PA1 14U // PORTA pin 1.
#define PA2 15U // PORTA pin 2.
#define PA3 16U // PORTA pin 3.
#define PA4 17U // PORTA pin 4.
#define PA5 18U // PORTA pin 5.
#define PA6 19U // PORTA pin 6.
#define PA7 20U // PORTA pin 7.

/**
 * @brief Initialize all digital output pins.
 */
void digital_out_setup(void);

/**
 * @brief Get state of the given pin.
 *
 * @param[in] pin Pin to read.
 *
 * @return State of the given pin (true = on, false = off).
 */
bool digital_out_read(uint8_t pin);

/**
 * @brief Set state of the given pin.
 *
 * @param[in] pin Pin to set.
 * @param[in] state New state (true = on, false = off).
 */
void digital_out_write(uint8_t pin, bool state);

/** @brief Enable PC0. */
void digital_out_PC0_on(void);

/** @brief Disable PC0. */
void digital_out_PC0_off(void);

/** @brief Enable PC1. */
void digital_out_PC1_on(void);

/** @brief Disable PC1. */
void digital_out_PC1_off(void);

/** @brief Enable PC2. */
void digital_out_PC2_on(void);

/** @brief Disable PC2. */
void digital_out_PC2_off(void);

/** @brief Enable PC3. */
void digital_out_PC3_on(void);

/** @brief Disable PC3. */
void digital_out_PC3_off(void);

/** @brief Enable PD1. */
void digital_out_PD1_on(void);

/** @brief Disable PD1. */
void digital_out_PD1_off(void);

/** @brief Enable PD2. */
void digital_out_PD2_on(void);

/** @brief Disable PD2. */
void digital_out_PD2_off(void);

/** @brief Enable PD3. */
void digital_out_PD3_on(void);

/** @brief Disable PD3. */
void digital_out_PD3_off(void);

/** @brief Enable PD4. */
void digital_out_PD4_on(void);

/** @brief Disable PD4. */
void digital_out_PD4_off(void);

/** @brief Enable PD5. */
void digital_out_PD5_on(void);

/** @brief Disable PD5. */
void digital_out_PD5_off(void);

/** @brief Enable PD6. */
void digital_out_PD6_on(void);

/** @brief Disable PD6. */
void digital_out_PD6_off(void);

/** @brief Enable PD7. */
void digital_out_PD7_on(void);

/** @brief Disable PD7. */
void digital_out_PD7_off(void);

/** @brief Enable PF0. */
void digital_out_PF0_on(void);

/** @brief Disable PF0. */
void digital_out_PF0_off(void);

/** @brief Enable PF1. */
void digital_out_PF1_on(void);

/** @brief Disable PF1. */
void digital_out_PF1_off(void);

/** @brief Enable PA0. */
void digital_out_PA0_on(void);

/** @brief Disable PA0. */
void digital_out_PA0_off(void);

/** @brief Enable PA1. */
void digital_out_PA1_on(void);

/** @brief Disable PA1. */
void digital_out_PA1_off(void);

/** @brief Enable PA2. */
void digital_out_PA2_on(void);

/** @brief Disable PA2. */
void digital_out_PA2_off(void);

/** @brief Enable PA3. */
void digital_out_PA3_on(void);

/** @brief Disable PA3. */
void digital_out_PA3_off(void);

/** @brief Enable PA4. */
void digital_out_PA4_on(void);

/** @brief Disable PA4. */
void digital_out_PA4_off(void);

/** @brief Enable PA5. */
void digital_out_PA5_on(void);

/** @brief Disable PA5. */
void digital_out_PA5_off(void);

/** @brief Enable PA6. */
void digital_out_PA6_on(void);

/** @brief Disable PA6. */
void digital_out_PA6_off(void);

/** @brief Enable PA7. */
void digital_out_PA7_on(void);

/** @brief Disable PA7. */
void digital_out_PA7_off(void);

#endif /** DIGITAL_OUT_H_ */
