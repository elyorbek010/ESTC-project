/** @file
 *
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example
 * @brief Blinky Example Application main file.
 *
 * This file contains the source code for a sample application to blink LEDs.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"

#define led_R 8
#define led_G NRF_GPIO_PIN_MAP(1, 9)
#define led_B 12
#define button NRF_GPIO_PIN_MAP(1, 6)
/**
 * @brief Function for application main entry.
 */
int main(void)
{
    nrf_gpio_cfg_input(button, NRF_GPIO_PIN_PULLDOWN); // Configure P1.6(SW1) as pull-up input
    nrf_gpio_cfg_output(led_R);   // Configure P0.8(LED2_R) as output
    nrf_gpio_cfg_output(led_G);   // Configure P1.9(LED2_G) as output
    nrf_gpio_cfg_output(led_B);   // Configure P0.12(LED2_B) as output

    nrf_gpio_pin_write(led_R, 1);
    nrf_gpio_pin_write(led_G, 1);
    nrf_gpio_pin_write(led_B, 1);

    int cnt = 0, arr[] = {led_R, led_R, led_G, led_G, led_G, led_B, led_B}; // RRGGGBB
    int size = sizeof(arr) / sizeof(arr[0]);

    while(true){
        if(nrf_gpio_pin_read(button) == 0){
            nrf_gpio_pin_write(arr[cnt], 0);
            nrf_delay_ms(500);
            nrf_gpio_pin_write(arr[cnt], 1);
            nrf_delay_ms(500);
            cnt++;
        }
        if(cnt == size) {
            cnt = 0;
        }
    }
}
