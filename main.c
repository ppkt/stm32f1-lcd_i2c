#include "usart.h"
#include "i2c_dma.h"
#include "hd44780-i2c.h"
#include "utils.h"
#include <stdio.h>

void NVIC_Configuration(void);


int main(void)
{
	USART1_Init(9600);

	NVIC_Configuration();
	LED_Init();
	I2C_LowLevel_Init(I2C1);

	printf("System ready\r\n");

	hd44780_init(TIM2);
    while(1)
    {

    }
}

void NVIC_Configuration(void)
{

    /* 1 bit for pre-emption priority, 3 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    NVIC_SetPriority(I2C1_EV_IRQn, 0x00);
    NVIC_EnableIRQ(I2C1_EV_IRQn);

    NVIC_SetPriority(I2C1_ER_IRQn, 0x01);
    NVIC_EnableIRQ(I2C1_ER_IRQn);


    NVIC_SetPriority(I2C2_EV_IRQn, 0x00);
    NVIC_EnableIRQ(I2C2_EV_IRQn);

    NVIC_SetPriority(I2C2_ER_IRQn, 0x01);
    NVIC_EnableIRQ(I2C2_ER_IRQn);

}
