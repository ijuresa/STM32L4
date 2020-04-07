/***********************************************************************************************//**
 * MIT License
 * 
 * Copyright (c) 2020 ivan.juresa
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **************************************************************************************************
 * @file   main.c
 * @author ivan.juresa
 * @brief  Program will initialize 2 GPIO pins on STM32L4 (LEDs) and turn them ON - OFF with
 *         predefined period using SysTick Timer.
 **************************************************************************************************/

/***************************************************************************************************
 *                      INCLUDE FILES
 **************************************************************************************************/
#include "stm32l475xx.h"
#include "typedefs.h"

/***************************************************************************************************
 *                      PRIVATE DEFINES
 **************************************************************************************************/
//! Default is Polling
#define SYSTEM_TIMER_INTERRUPT (TRUE)

//! Registers
#define SYST_CSR   (0xE000E010u) //!< SysTick Control and Status Register
#define SYST_RVR   (0xE000E014u) //!< SysTick Reload Value Register
#define SYST_CVR   (0xE000E018u) //!< SysTick Current Value Register
#define SYST_CALIB (0xE000E01Cu) //!< SysTick Calibration Value Register

//! SYS_CSR
#define SYST_CSR_ENABLE    (1u << 0u)  //!< Enable counter
#define SYST_CSR_TICKINT   (1u << 1u)  //!< Counting down will assert SysTick exception request
#define SYST_CSR_CLKSOURCE (1u << 2u)  //!< Use processor clock
#define SYST_CSR_COUNTFLAG (1u << 16u) //!< Returns 1 if timer is finished

/***************************************************************************************************
 *                      PRIVATE DATA TYPES
 **************************************************************************************************/

/***************************************************************************************************
 *                      PRIVATE VARIABLES
 **************************************************************************************************/
// SysTick
static volatile uint32_t * const sysCsr = (volatile uint32_t *)SYST_CSR;
static volatile uint32_t * const sysRvr = (volatile uint32_t *)SYST_RVR;
static volatile uint32_t * const sysCvr = (volatile uint32_t *)SYST_CVR;

//! Current LEDs status
static bool_t state = TRUE; //!< ON

/***************************************************************************************************
 *                      GLOBAL VARIABLES DEFINITION
 **************************************************************************************************/

/***************************************************************************************************
 *                      PRIVATE FUNCTION DECLARATION
 **************************************************************************************************/
static void ConfigureSystemClock(void);
static void ConfigureLeds(void);
static void TurnOff(void);
static void TurnOn(void);

/***************************************************************************************************
 *                      PUBLIC FUNCTIONS DEFINITION
 **************************************************************************************************/
int main(void) {
    ConfigureSystemClock();
    ConfigureLeds();

    while(TRUE) {
#if (SYSTEM_TIMER_INTERRUPT != TRUE)
        // Check if SysTick counted to 0
        if((*sysCsr & SYST_CSR_COUNTFLAG) == SYST_CSR_COUNTFLAG) {
            // Change state
            state = ((~state) & 0x01u);
        }
#endif // SYSTEM_TIMER_INTERRUPT

        if(state != TRUE) {
            TurnOff();
        } else {
            TurnOn();
        }
    }

    // Should never get here
    return 0;
}

#if (SYSTEM_TIMER_INTERRUPT == TRUE)
void SysTick_Handler(void) {
    // Change state
    state = ((~state) & 0x01u);
}
#endif // SYSTEM_TIMER_INTERRUPT

/***************************************************************************************************
 *                      PRIVATE FUNCTIONS DEFINITION
 **************************************************************************************************/
static void ConfigureSystemClock(void) {
    // Set current value to 0
    *sysCvr = 0u;

    // Reload value is 4MHz (1second)
    *sysRvr = (4000000u - 1u); // N - 1

#if (SYSTEM_TIMER_INTERRUPT == TRUE)
    // Enable SysTick Timer, Interrupt and set Processor Clock as a source
    *sysCsr |= (SYST_CSR_ENABLE | SYST_CSR_TICKINT | SYST_CSR_CLKSOURCE);
#else
    // Enable SysTick Timer and set Processor Clock as a source
    *sysCsr |= (SYST_CSR_ENABLE | SYST_CSR_CLKSOURCE);
#endif // SYSTEM_TIMER_INTERRUPT
}

static void ConfigureLeds(void) {
    /* RCC */
    // Enable peripheral clock for LED ports
    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN);

    /* MODE */
    // Configure LEDs as an outputs. As we are writing values (To turn the LED ON)
    GPIOA->MODER = (GPIOA->MODER & (~GPIO_MODER_MODE5)) | GPIO_MODER_MODE5_0;
    GPIOB->MODER = (GPIOB->MODER & (~GPIO_MODER_MODE14)) | GPIO_MODER_MODE14_0;

    /* OUTPUT TYPE */
    // By default it is in Output push-pull.
    // The one we need so we can:
    //   - Pull to ground (Turn OFF)
    //   - Push to power supply voltage (Turn ON)

    /* SPEED */
    // Put it to low speed
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5_0;
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED14_0;

    /* PULL UP/DOWN */
    // Lets pull down both LEDs so by default they are OFF and signal is not floating
    GPIOA->PUPDR = (GPIOA->PUPDR & (~GPIO_PUPDR_PUPD5)) | GPIO_PUPDR_PUPD5_1;
    GPIOB->PUPDR = (GPIOB->PUPDR & (~GPIO_PUPDR_PUPD14)) | GPIO_PUPDR_PUPD14_1;
}

static void TurnOff(void) {
    // Clear bits
    GPIOA->ODR &= (~GPIO_ODR_OD5);
    GPIOB->ODR &= (~GPIO_ODR_OD14);
}

static void TurnOn(void) {
    // Set bits
    GPIOA->ODR |= GPIO_ODR_OD5;
    GPIOB->ODR |= GPIO_ODR_OD14;
}

/***************************************************************************************************
 *                      END OF FILE
 **************************************************************************************************/
