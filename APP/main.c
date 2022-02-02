/**************************************************************************************************************************************
* LOG:                                                                   							   								  *
* VERSION					AUTHOR           					DATE 						DESCRIPTION 		   					  *
* 1.0.0 					Kareem Hassaan						29 JAN,2022					- Initial Creation						  *
**************************************************************************************************************************************/
/**************************************************************************************************************************************
* ! Title      	: Stop Watch                                                        							                      *
* ! File Name	: main.c                                                  		     							                      *
* ! Description : Stop Watch Based on Six Multiplexed 7-Segment using Timer         							                      *
* ! Compiler  	: GNU AVR cross Compiler                                            							                      *
* ! Target 	  	: Atmega32 Micro-Controller                                         							                      *
* ! Layer 	  	: APP                  						                                         								  *
**************************************************************************************************************************************/

/**************************************************** Library Inclusions *************************************************************/
#define F_CPU 8000000UL
#include<util/delay.h>
#include"../LIB/STD_TYPES.h"
#include"../LIB/BIT_MATH.h"
/************************************************** Lower Layer Inclusions ***********************************************************/
#include"../HAL/04-Seven_Segments/SevenSeg_intrface.h"

#include"../MCAL/05-Timers/TIMERS_intrface.h"
#include"../MCAL/01-DIO/DIO_intrface.h"
#include"../MCAL/03-GIE/GIE_intrface.h"
/********************************************************** Macros *******************************************************************/
#define DIGIT_NUM						  3
#define CONROL_PORT						  PORTB
#define BUTTONS_PORT					  PORTD

#define CENTI_SEC_SEVEN_SEG_1			  PIN0
#define CENTI_SEC_SEVEN_SEG_2			  PIN1
#define SEC_SEVEN_SEG_1					  PIN2
#define SEC_SEVEN_SEG_2					  PIN3
#define MIN_SEVEN_SEG_1					  PIN4
#define MIN_SEVEN_SEG_2					  PIN5

#define CLEAR_BUTTON					  PIN0
#define RESUME_BUTTON					  PIN1
#define STOP_BUTTON						  PIN2

#define MINIUTS							   0
#define SECONDS				  			   1
#define CENTI_SECONDS			 		   2

#define ONE_MINIUT						   60
#define ONE_SECOND				  		   99
/***************************************************** Global Variables **************************************************************/
volatile uint8 GlobalArr_StopWatch[DIGIT_NUM] = {0};
/*************************************************** Functions Prototype *************************************************************/
void CBKFunc_StopWatch();

/*************************************************************************************************************************************/
/******************************************************* Main Function ***************************************************************/
/*************************************************************************************************************************************/
void main()
{
	/*Enable The Global Interrupt*/
	MGIE_voidGlobalInterruptEnable();
	/*Setting Timer0 Call Back Function*/
	MTIMERS_voidSetCallBack(TIMER0, OVERFLOW, &CBKFunc_StopWatch);

	/*Initializ The Seven Segment*/
	HSevenSegment_VoidInit();

	/*Initializ The Control Port to Be output*/
	MDIO_voidSetPortDirection(CONROL_PORT, ALL_OUTPUT);

	/*Initializ The Control Buttons Pins to Be Input*/
	MDIO_voidSetPinDirection(PORTD, CLEAR_BUTTON , INPUT);
	MDIO_voidSetPinDirection(PORTD, RESUME_BUTTON, INPUT);
	MDIO_voidSetPinDirection(PORTD, STOP_BUTTON  , INPUT);
	/*Pull up The Control Buttons Pins*/
	MDIO_voidSetPinValue(PORTD, CLEAR_BUTTON , HIGH);
	MDIO_voidSetPinValue(PORTD, RESUME_BUTTON, HIGH);
	MDIO_voidSetPinValue(PORTD, STOP_BUTTON  , HIGH);
	/*Initialize Timer and Enable it*/
	MTIMERS_VoidTimer0Init();


	while(1)
	{
		/*Checking If The Clear Button Pressed*/
		if( MDIO_uint8GetPinValue(BUTTONS_PORT, CLEAR_BUTTON) == 0)
		{
			/*Clearing All Digits*/
			GlobalArr_StopWatch[MINIUTS] = 0;
			GlobalArr_StopWatch[SECONDS] = 0;
			GlobalArr_StopWatch[CENTI_SECONDS] = 0;
		}
		/*Checking If The Clear Resume Pressed*/
		if( MDIO_uint8GetPinValue(BUTTONS_PORT, RESUME_BUTTON) == 0)
		{
			/*Enable The Timer To Resume*/
			MTIMERS_VoidTimer0Enable();
		}
		/*Checking If The Clear Stop Pressed*/
		if( MDIO_uint8GetPinValue(BUTTONS_PORT, STOP_BUTTON) == 0)
		{
			/*Disable The Timer To Stop*/
			MTIMERS_VoidTimer0Disable();
		}


		/*Selecting First Centi Sec 7-seg and Send to it the tenth digit*/
		MDIO_voidSetPortValue(CONROL_PORT, ~(HIGH << CENTI_SEC_SEVEN_SEG_1));
		HSevenSegment_VoidDisplayNumber((GlobalArr_StopWatch[CENTI_SECONDS]%10));

		_delay_ms(1);

		/*Selecting Second Centi Sec 7-seg and Send to it the Hundredth digit*/
		MDIO_voidSetPortValue(CONROL_PORT, ~(HIGH << CENTI_SEC_SEVEN_SEG_2));
		HSevenSegment_VoidDisplayNumber((GlobalArr_StopWatch[CENTI_SECONDS]/10));

		_delay_ms(1);

		/*Selecting First Sec 7-seg and Send to it the tenth digit*/
		MDIO_voidSetPortValue(CONROL_PORT, ~(HIGH << SEC_SEVEN_SEG_1));
		HSevenSegment_VoidDisplayNumber((GlobalArr_StopWatch[SECONDS]%10));

		_delay_ms(1);

		/*Selecting Second Sec 7-seg and Send to it the Hundredth digit*/
		MDIO_voidSetPortValue(CONROL_PORT, ~(HIGH << SEC_SEVEN_SEG_2));
		HSevenSegment_VoidDisplayNumber((GlobalArr_StopWatch[SECONDS]/10));

		_delay_ms(1);

		/*Selecting First Min 7-seg and Send to it the tenth digit*/
		MDIO_voidSetPortValue(CONROL_PORT, ~(HIGH << MIN_SEVEN_SEG_1));
		HSevenSegment_VoidDisplayNumber((GlobalArr_StopWatch[MINIUTS]%10));

		_delay_ms(1);

		/*Selecting Second Min 7-seg and Send to it the Hundredth digit*/
		MDIO_voidSetPortValue(CONROL_PORT, ~(HIGH << MIN_SEVEN_SEG_2));
		HSevenSegment_VoidDisplayNumber((GlobalArr_StopWatch[MINIUTS]/10));

		_delay_ms(1);

	}
}

/*************************************************************************************************************************************/
/**************************************************** Timer CBK Function *************************************************************/
/*************************************************************************************************************************************/
void CBKFunc_StopWatch()
{
	/*Counting One Centi Second*/
	GlobalArr_StopWatch[CENTI_SECONDS]++;

	/*Counting One Second*/
	if(GlobalArr_StopWatch[CENTI_SECONDS] == ONE_SECOND)
	{
		GlobalArr_StopWatch[SECONDS]++;
		GlobalArr_StopWatch[CENTI_SECONDS] = 0;
	}

	/*Counting One Minute*/
	if(GlobalArr_StopWatch[SECONDS] == ONE_MINIUT)
	{
		GlobalArr_StopWatch[MINIUTS]++;
		GlobalArr_StopWatch[SECONDS] = 0;
	}
}
