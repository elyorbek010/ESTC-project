
/*
You must include these files

 */
#include <stdio.h>
#include <string.h>
#include "nrf_drv_pwm.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"


/* these headers are important for nrf_logger*/
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"




// Set the top value which will define the frequency of the pulses
#define m_top     10000 // here we are using 1 Mhz frequency so if we divide 1 Mhz by 10000 we get 100... so 100 pulses per second which means each pulse is of 10ms
#define m_step    100 // this is the step value we will use this to increase or decrease the period of the pulses

#define  led_external  22 // using an external pin for pwm which is not defined in bsp

// create a a struct handle which will refer to the pwm instance.
// for nrf52832 it can be 0, 1, 2
// for nrf52840 it can be 0, 1, 2, 3
static nrfx_pwm_t m_pwm0 = NRFX_PWM_INSTANCE(0);



// we will use this variable to create a signle step
// Note: we can use any value from 0 upto the m_top value
uint16_t step = m_top / m_step;

// Since we are using common loading mode so whatever values we load in the pwm instance they will be loaded to all the channels
// create a sequence value array to hold multiple values
// they will be loaded from the ram using dma so the processor is mostly free during this operation

// Note: more values use more ram so make sure you optimize the total required values, if you can do the same task in 200 values then
// don't use 1000 values 
static nrf_pwm_values_common_t sequence_values[m_step * 2]; // here m_step = 100 so m_step*2 means 200 values

// we will use a uint16_t value to save it to the sequence because the sequence values are half-word which is 16 bits or 2 bytes
uint16_t value = 0;



// A function that will initialize the pwm with common values
static void pwm_common_init(void)
{

	// for loop will be used to store these sequences
	// it will use a simple algorithm to store 200 values in 100 loops
    for(int i = 0; i<m_step; i++)
    {
		
      value += step; // >loop1:  0 + 100 = 100  		>loop2:  100 + 100 = 200    		>loop3: 200 + 100 = 300 ....
      sequence_values[i] = value;
      sequence_values[m_step + i] = m_top - value; //  >loop1: sequence_values[100 + 0] = 10,000 - 0 = 10,000 		>loop2: sequence_values[100 + 1] = 10,000 - 100 > 9900 ....
    }



// Create a config struct and assign it the configuration values for your pwm instance
    nrfx_pwm_config_t const config0 = 
    {
      .output_pins = /* connect the channels to pins in a sequence from channel 0 to channel 3*/
      {
          BSP_LED_0 | NRFX_PWM_PIN_INVERTED, // pin connected with channel 1
          led_external, // pin connected with channel 2
          NRFX_PWM_PIN_NOT_USED, // if you don't want to use the pin just use this constant
          NRFX_PWM_PIN_NOT_USED // make sure there is no comma {,} at the end of last channel assignment
      },
      .irq_priority = APP_IRQ_PRIORITY_LOWEST, // set the interrupt event priority Note: interrupt events are dealt in next tutorial
	  //we have different options upto 16Mhz for more info just right click this line in segger embedded studio and click on goto the definition to see more available options
      .base_clock   = NRF_PWM_CLK_1MHz, // Set the input frequency of the pwm instance, 
      .count_mode   = NRF_PWM_MODE_UP, // This is the counter mode, we use mode up so the counter will start from 0 to the m_top value and then reset to 0
      .top_value    = m_top, // This value defines the frequency of pulses that is explained at line 26
      .load_mode    = NRF_PWM_LOAD_COMMON, // we are using the Common mode so we will use this constant to tell the module that we are using common mode
      .step_mode    = NRF_PWM_STEP_AUTO

    };

// first parameter is the pointer to driver instance which is m_pwm0, second parameter is the configurations, last parameter is the event handler, we will not use it in this tutorial
// so we will pass a NULL value
    APP_ERROR_CHECK(nrfx_pwm_init(&m_pwm0, &config0, NULL)); // we are directly passing the error to APP_ERROR_CHECK function while initializng the pwm
}



// Create a function which will play the sequence of values that were created above
static void pwm_play(void)
{
// we have to define a struct which will hold the sequence along with its properties 
	
  nrf_pwm_sequence_t const seq0 = 
  {
    .values  = {sequence_values}, // pass the pointer to sequence values, array name itself is a pointer so we used that
    .length           = NRF_PWM_VALUES_LENGTH(sequence_values), // we will use an internal function to calculate the number of values or we can directly write the number of values in the sequence
    .repeats          = 0, // we can mention to repeat each duty cycle value as many times as we want (Note: limit is a 32-bit value)
    .end_delay        = 0 // additional time the last duty cycle is to be kept 
  
  };

// we use (void) to throw off the value received so if an error occurs we will not receive it 
// use it only in testing but a common practice is to use APP_ERROR_CHECK function along with error code variable
// this function will play the sequence of values 1st parameter is pwm pointer, second is the struct pointer that we created above,
// 3rd is the number of times we want to repeat a duty cycle, 4th parameter is the flag to tell to repeat above sequences indifinitely or stop after played once
//
  (void)nrfx_pwm_simple_playback(&m_pwm0, &seq0, 1, NRFX_PWM_FLAG_LOOP);

  }



// A funtion to initialize the logger module for more info see the nrf_logger tutorial
static void log_init(void)
{
  ret_code_t err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_DEFAULT_BACKENDS_INIT();

  }





// main function 
int main(void)
{

// Initialize the logger module
log_init();

// Initialize the leds and use them for other purposes as well if you want to if not then we don't need to initialize the pins for pwm 
// they will be automatically initialized
bsp_board_init(BSP_INIT_LEDS);

// call the initialization function
pwm_common_init();

// Play the sequence values by calling the play function
pwm_play();


// Display a start message  
  NRF_LOG_INFO("PWM application started!!");

  while(true)
  {
   // you can perform other tasks as pwm is handled without the need of processor so processor is free most of its time
  }
   
}


/** @} */
