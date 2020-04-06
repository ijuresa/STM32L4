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
 *         predefined period using processor blocking FOR loop.
 **************************************************************************************************/

/***************************************************************************************************
 *                      INCLUDE FILES
 **************************************************************************************************/
#include "stm32l475xx.h"
#include "typedefs.h"

/***************************************************************************************************
 *                      PRIVATE DEFINES
 **************************************************************************************************/
#define DELAY (400000u)

/***************************************************************************************************
 *                      PRIVATE DATA TYPES
 **************************************************************************************************/

/***************************************************************************************************
 *                      PRIVATE VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 *                      GLOBAL VARIABLES DEFINITION
 **************************************************************************************************/

/***************************************************************************************************
 *                      PRIVATE FUNCTION DECLARATION
 **************************************************************************************************/
static void ConfigureLeds(void);
static void TurnOff(void);
static void TurnOn(void);
static void Delay(void);

/***************************************************************************************************
 *                      PUBLIC FUNCTIONS DEFINITION
 **************************************************************************************************/
int main(void) {
    ConfigureLeds();

    while(TRUE) {
        TurnOn();
        Delay();

        TurnOff();
        Delay();
    }

    // Should never come here
    return 0;
}

/***************************************************************************************************
 *                      PRIVATE FUNCTIONS DEFINITION
 **************************************************************************************************/
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

static void Delay(void) {
    uint32_t i;

    for(i = 0; i < DELAY; i ++) { }
}

/***************************************************************************************************
 *                      END OF FILE
 **************************************************************************************************/
