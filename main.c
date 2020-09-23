/*
September 15, 2020
ATMEGA32 MQ135 Air Quality Sensor

The levels of CO2 in the air and potential health problems are:

250 - 400 ppm: background (normal) outdoor air level.
400 - 1,000 ppm: typical level found in occupied spaces with good air exchange.
1,000 - 2,000 ppm: level associated with complaints of drowsiness and poor air.
2,000 - 5,000 ppm: level associated with headaches, sleepiness, and stagnant, stale, stuffy air. Poor concentration, loss of attention, increased heart rate and slight nausea may also be present.
5,000 ppm: this indicates unusual air conditions where high levels of other gases could also be present. Toxicity or oxygen deprivation could occur. This is the permissible exposure limit for daily workplace exposures.
40,000 ppm: this level is immediately harmful due to oxygen deprivation.

*/
#define F_CPU 8000000UL //8Mhz CPU
#define RLOAD 22.0 //Resistor put on the MQ135 instead of original 1K its now 22K
#define RZERO 60.52
#include <avr.c>
#include <lcd.c> // 2x16 LCD
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <MQ135.h>

unsigned char _DEBUG = 0; //Toggle debug  

unsigned short measurements[120];
unsigned short one_hr_measurements[60];
unsigned short twenty_four_hr_measurements[24];
unsigned short rzero_measurements[30];
unsigned short high = 0, low = 8888, hours_elapsed = 0, counter = 0, rzerocounter = 0;
float one_min_avg = 0, one_hr_avg = 0, twenty_four_hr_avg = 0;
unsigned char been_min = 0, been_hour = 0, been_day = 0;
float rzero = 0;

//Clock
unsigned char sec = 0, min = 0, hours = 0;

/* LCD lines 1 and 2 */
char line1[17]; 
char line2[17];

int ADC_Read(char channel)
{
	int Ain,AinLow;
	
	ADMUX=ADMUX|(channel & 0x0f);	/* Set input channel to read */

	ADCSRA |= (1<<ADSC);		/* Start conversion */
	while((ADCSRA&(1<<ADIF))==0);	/* Monitor end of conversion interrupt */
	
	_delay_us(10);
	AinLow = (int)ADCL;		/* Read lower byte*/
	Ain = (int)ADCH*256;		/* Read higher 2 bits and 
					Multiply with weight */
	Ain = Ain + AinLow;				
	return(Ain);			/* Return digital value*/
}

void averages(){
	one_min_avg = 0;
	short i;
	for (i = 0; i < 120; i++)
	{
		one_min_avg += measurements[i];
	}
	one_min_avg = one_min_avg/120.;
	one_hr_measurements[min] = one_min_avg;
	
	if (been_hour == 1 && sec == 0){ //Do calc every minute
		for (i = 0; i < 60; ++i)
		{
			one_hr_avg += one_hr_measurements[i];
		}
		one_hr_avg = one_hr_avg / 60.;
		twenty_four_hr_measurements[hours] = one_hr_avg;
				
		//Average for the 24 hours
		if (been_day == 1 && hours == 0) //Do calc once an hour
		{
			for (i = 0; i < 24; ++i)
			{
				twenty_four_hr_avg += twenty_four_hr_measurements[i];
			}
			twenty_four_hr_avg = twenty_four_hr_avg / 24.;
		}
	}
	
}

void clock(){
	sec++;
	if (sec >= 60){
		been_min = 1;
		sec = 0;
		min += 1;
		if (min >= 60){
			been_hour = 1;
			min = 0;
			hours += 1;
			hours_elapsed += 1;
			if (hours == 24) {
				been_day = 1;
				hours = 0;
			}
		}
	}
	averages();	
}

void light_up_led(){
	
}

/* PORTED CODE FROM https://github.com/GeorgK/MQ135 */

/**************************************************************************/
/*!
@brief  Get the resistance of the sensor, ie. the measurement value

@return The sensor resistance in kOhm
*/
/**************************************************************************/
float getResistance() {
	int val = ADC_Read(4);
	return ((1023./(float)val) - 1.)*RLOAD;
}

/**************************************************************************/
/*!
@brief  Get the resistance RZero of the sensor for calibration purposes

@return The sensor resistance RZero in kOhm
*/
/**************************************************************************/
float getRZero() {
	return getResistance() * pow((ATMOCO2/PARA), (1./PARB));
}

/**************************************************************************/
/*!
@brief  Get the ppm
*/
/**************************************************************************/
float getPPM() {
	return PARA * pow((getResistance()/RZERO), -PARB);
}


/* END IMPORTED CODE */

void warm_up(){ // Runs once on boot to warm up sensor
	char counter = 120;
	while (counter > 0){
		sprintf(line1, "Warming Up: %d",counter);
		sprintf(line2, "PPM: %.1f", getPPM());
		clr_lcd();
		puts_lcd2(line1);
		pos_lcd(1,0);
		puts_lcd2(line2);
		_delay_us(1000000);
		counter--;
		
		//Alternate LEDS while warming up
		if (counter % 2 == 0)
		{ 
			SET_BIT(PORTC,7); //Red
			_delay_us(240);
			CLR_BIT(PORTC,7); //Red
		}
		else
		{
			SET_BIT(PORTC,6); //Yellow
			_delay_us(240);
			CLR_BIT(PORTC,6); //Yellow
		}
		
	}
	measurements[0] = getPPM(); //Baseline for Hi/Lo
}

void debug()
{

	//Rzero averaging
	if (been_min == 0) 
	{
		rzero = getRZero(); //If you want to print floats need to add -lprintf_flt under Misc in project settings and enable vprintf library under General more info https://startingelectronics.org/articles/atmel-AVR-8-bit/print-float-atmel-studio-7/
		rzero_measurements[rzerocounter] = rzero;
		rzerocounter++;
		if (rzerocounter >= 30) rzerocounter = 0;
	}
	else
	{
		rzero = 0;
		for (unsigned char i = 0; i < 30; ++i)
		{
			rzero += rzero_measurements[i];
		}
		rzero = rzero / 30;
		rzero_measurements[rzerocounter] = getRZero();
		rzerocounter++;
		if (rzerocounter >= 30) rzerocounter = 0;
	}
	sprintf(line1, "PPM:%.1f r%.1f", getPPM(), getResistance());
	sprintf(line2, "RAW:%i R0:%.2f", ADC_Read(4), rzero);
	
}


int main(void)
{
	ini_lcd();
	
	DDRA=0x0;			/* Make ADC ports as input */
	DDRC=0xFF;         /* Make C ports as output for LEDs */
	SET_BIT(DDRC,7);
	SET_BIT(DDRC,6); 
	
	ADCSRA = 0x87;		/* Enable ADC, fr/128  */
	ADMUX = 0x40;		/* Vref: Avcc, ADC channel: 0 */
	if (_DEBUG == 0) warm_up();
	counter = 0;
	while (1)
	{
		if (_DEBUG == 0)
		{
			if (been_min == 0) sprintf(line1, "PPM:%.1f Hr: --", getPPM());
			else if (been_hour == 1){
				sprintf(line1, "PPM:%i H:%.1f", (unsigned int)one_min_avg, one_hr_avg);
			}
			else {
				sprintf(line1, "PPM:%d Hr: --", (unsigned int)one_min_avg);
			}
		
			//Line 2
			if (been_day == 1 && (sec < 15 || (sec > 30 && sec < 45))) sprintf(line2, "24H Avg: %.1f", twenty_four_hr_avg);
			else if (been_day == 1 && (sec >= 45 || (sec >= 15 && sec <= 30))) 
			{
				sprintf(line2, "%iH Hi:%i Lo:%i", hours_elapsed, high, low);
				if (strlen(line2) > 16) sprintf(line2, "%iH Hi%i L%i", hours_elapsed, high, low);
			}
			else if (been_min == 1) 
			{
				sprintf(line2, "%iH Hi:%i Lo:%i", hours_elapsed, high, low);
				if (strlen(line2) > 16) sprintf(line2, "%iH Hi%i L%i", hours_elapsed, high, low);
			}
		}
		else debug();
		
		clr_lcd();
		puts_lcd2(line1);
		pos_lcd(1,0);
		puts_lcd2(line2);
		
		// LOGIC TO LOWER LED VOLTAGE I NEEDED THIS TO PREVENT 2.2v LEDS from BURNING OUT at 5v, below logic delivers ~2v to LEDS
				
		//Wait a bit
		if (_DEBUG == 0)
		{
			if (one_min_avg <= 1000) //LEDS OFF
			{
				_delay_us(500000); //Read every 0.5 sec, u_sec is 1/1 millionth of a sec
			}
			else if (one_min_avg > 1000 && one_min_avg < 1500) //Yellow LED Blink
			{
				float wait_timer = 2000;
					while (wait_timer > 0)
					{
						if(sec % 2 == 0)
						{
							SET_BIT(PORTC,6); //Yellow
							_delay_us(240);
							CLR_BIT(PORTC,6); //Yellow
						}
						else _delay_us((240));
						wait_timer--;
					}
			}
			else if (one_min_avg >= 1500 && one_min_avg < 2000) //Yellow LED ON
			{
				float wait_timer = 2000;
				while (wait_timer > 0){
					SET_BIT(PORTC,6); //Yellow
					_delay_us(240);
					CLR_BIT(PORTC,6); //Yellow
					wait_timer--;
				}
			}
			else if (one_min_avg >= 2000 && one_min_avg < 5000) //Red LED ON
			{
				float wait_timer = 2000;
				while (wait_timer > 0){
					SET_BIT(PORTC,7); //Red
					_delay_us(240);
					CLR_BIT(PORTC,7); //Red
					wait_timer--;
				}
			}
			else if (one_min_avg >= 5000) //Red LED on and Yellow flashing
			{
				float wait_timer = 1000;
				while (wait_timer > 0)
				{
					if (sec % 2 == 0)
					{
						SET_BIT(PORTC,6); //Yellow
						_delay_us(240);
						CLR_BIT(PORTC,6); //Yellow
					}
					else _delay_us((240));
					SET_BIT(PORTC,7); //Red
					_delay_us(240);
					CLR_BIT(PORTC,7); //Red
					wait_timer--;
				}
			}
		}
		/*
		else //Debug mode both LEDs are blinking every 10 sec
		{
			float wait_timer = 2000;
			while (wait_timer > 0)
			{
				if (sec % 10 == 0){
					SET_BIT(PORTC,7); //Red
					SET_BIT(PORTC,6); //Yellow
					_delay_us(240);
					CLR_BIT(PORTC,7); //Red
					CLR_BIT(PORTC,6); //Yellow
				}
				else _delay_us(240);
				wait_timer--;
			}
		}
		*/
		measurements[counter] = getPPM();
		//High,low
		if (been_min == 1)
		{
			if (one_min_avg > high) high = one_min_avg;
			if (one_min_avg < low ) low = one_min_avg;
		}
		
		counter++;
		if (counter % 2 == 0) clock();
		if (counter >= 120) counter = 0;
	}
	return 0;

}


