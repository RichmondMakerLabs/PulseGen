#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include "gpio.h"

/* Pulse generator parameters */
/* All time values are in microseconds */
#define PULSE_ON 1
#define PULSE_OFF 0

#define ON_TIME_MAX 500
#define ON_TIME_DEFAULT 50
#define ON_TIME_MIN 25
#define ON_TIME_INCREMENT 1
/* on time is equal to the off time */
#define OFF_TIME_MAX ON_TIME_MAX
#define OFF_TIME_DEFAULT ON_TIME_DEFAULT
#define OFF_TIME_MIN ON_TIME_MIN
#define OFF_TIME_INCREMENT ON_TIME_INCREMENT

#define N_PULSES_MAX 25
#define N_PULSES_DEFAULT 5
#define N_PULSES_MIN 1
#define N_PULSES_INCREMENT 1

#define BACKOFF_TIME_MAX 3000
#define BACKOFF_TIME_DEFAULT 500
#define BACKOFF_TIME_MIN 15
#define BACKOFF_TIME_INCREMENT 10

#define FINE_PULSE_WIDTH_MAX 6000
#define FINE_PULSE_WIDTH_ DEFAULT 0
#define FINE_PULSE_WIDTH_MIN 0
#define FINE_PULSE_WIDTH_INCREMENT 10

#define PUMP_ON 1
#define PUMP_OFF 0

#define EXIT  		'E'
#define ENABL_PULSE 	'1'
#define DIS_PULSE 	'2'
#define ENABL_PUMP 	't'
#define DIS_PUMP	'f'
#define INC_PULSE	'q'
#define DEC_PULSE	'a'
#define INC_ON_TIME	'o'
#define DEC_ON_TIME	'k'
#define INC_OFF_TIME	'i'
#define DEC_OFF_TIME	'j'
#define INC_BACKOFF_TIME  'u'	
#define DEC_BACKOFF_TIME  'h'
#define REFRESH_SCN	'X'

/* Pulse generator variables */
int onTime =  ON_TIME_DEFAULT;
int offTime = OFF_TIME_DEFAULT;
int numPulses = N_PULSES_DEFAULT;
int backoffTime = BACKOFF_TIME_DEFAULT;

/* Additional GPIO state variables */
int pulseState = PULSE_OFF;
int pumpState = PUMP_OFF;

/* frequency calculation functions */
float PulseTrainFreq()
{
	return 1000000/(onTime+offTime);
}

float Freq()
{
	return 1000000/(((numPulses*(onTime+offTime))-offTime)+backoffTime);
}

/* Send Data to GPIO to update */
//int Gpio()
//{
//return 0;
//}

void PrintScreen()
{	
	attron(A_BOLD | COLOR_PAIR(2));
	mvprintw(1,26,"Pulse Generator Control Screen ");
	mvprintw(2,26,"-------------------------------");
	attroff(A_BOLD | COLOR_PAIR(2));
	mvprintw(2,0,"Pulses:");
	mvprintw(3,0,"Pump  :");
	mvprintw(5,0,"PULSES");
	mvprintw(6,40,"Pulse on time (microseconds)  :");
	mvprintw(7,40,"Pulse off time (microseconds) :");
	mvprintw(7,0,"Number of pulses per cycle: ");
	mvprintw(8,0,"Pulse frequency Hz        :");
	mvprintw(8,40,"Tweak pulse width (fine)      :");
	mvprintw(10,0,"BACKOFF\n");
	mvprintw(11,6,"Backoff time (microseconds) :");
	mvprintw(12,0,"CYCLE\n");
	mvprintw(13,6,"Frequency Hz                :");
	attron(COLOR_PAIR(1));
	mvprintw(15,0,"KEYBOARD CONTROL\n");
	
	mvprintw(16,4,"P/L - inc/dec pulse speed");
	mvprintw(17,4,"O/K - inc/dec pulse on time");
	mvprintw(18,4,"I/J - inc/dec pulse off time");
	mvprintw(19,4,"U/H - inc/dec backoff time");
	mvprintw(20,4,"Q/A - inc/dec number of pulses");
	mvprintw(16,43,"Y/G - inc/dec fine pulse width");
	mvprintw(17,45,"R - Reset fine pulse tweak");
	mvprintw(22,4,"1 - START PULSES     2 - STOP PULSES");
	mvprintw(23,4,"T - PUMP ON          F - PUMP OFF\n");
	mvprintw(22,45,"%c - Refresh Screen", REFRESH_SCN);
	mvprintw(23,45,"%c - EXIT (UPPER-CASE %c)", EXIT, EXIT);
	attroff(COLOR_PAIR(1));
};

void PrintValues()
{
	if (pulseState) 
	{
		attron(A_BOLD | COLOR_PAIR(3));
		mvprintw(2, 9, "ON  ");
		attroff(A_BOLD | COLOR_PAIR(3));
	}
	else
	{
		attron(A_BOLD | COLOR_PAIR(4));
		mvprintw(2, 9, "OFF ");
		attroff(A_BOLD | COLOR_PAIR(4));
	}
	if (pumpState) 
	{
		attron(A_BOLD | COLOR_PAIR(3));
		mvprintw(3, 9, "ON  ");
		attroff(A_BOLD | COLOR_PAIR(3));
	}
	else
	{
		attron(A_BOLD | COLOR_PAIR(4));
		mvprintw(3, 9, "OFF ");
		attroff(A_BOLD | COLOR_PAIR(4));
	}
	attron(COLOR_PAIR(3));
	mvprintw(8,28,"%d   ", PulseTrainFreq());
	mvprintw(13,36,"%d   ", Freq());

	mvprintw(7,28,"%d  ",numPulses);
	mvprintw(6,72,"%d  ",onTime);
	mvprintw(7,72,"%d  ",offTime);
	mvprintw(11,36,"%d  ",backoffTime);
	attroff(COLOR_PAIR(3));
}

int main()

{	int  ch;
//	int row,col;
	Gpio_init();
	Gpio_timing();	// Initial (default) settings
	Gpio_pulse(pulseState);
	Gpio_pump(pumpState);
	initscr();				/* Start Curses Mode */
	start_color();				/* Assuming terminal can handle colour*/
	init_pair(1, COLOR_CYAN, COLOR_BLACK);  /* For blue type */
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);/* For yellow type */
	init_pair(3, COLOR_GREEN, COLOR_BLACK); /* For green type */
	init_pair(4, COLOR_RED, COLOR_BLACK);   /* For red type */
	raw();					/* Line buffering disabled */
	keypad(stdscr, TRUE);			/* We get F1, F2 etc.. */
	noecho();				/* Don't echo() while we do getch */
	curs_set(0);				/* Hide cursor */
//	getmaxyx(stdscr,row,col);

	PrintScreen();
	PrintValues();
/* Main loop */

	while ((ch = getch()) != EXIT)
	{  
		switch (tolower(ch))
		{
		case ENABL_PULSE:
				pulseState = PULSE_ON;
				PrintValues();
				Gpio_pulse(pulseState);		
		break;

		case DIS_PULSE:
				pulseState = PULSE_OFF;
				PrintValues();
				Gpio_pulse(pulseState);	
		break;
		
		case ENABL_PUMP:
				pumpState = PUMP_ON;
				PrintValues();
				Gpio_pump(pumpState);	
		break;
		
		case DIS_PUMP:
				pumpState = PUMP_OFF;
				PrintValues();
				Gpio_pump(pumpState);
		break;

		case INC_PULSE:	
			if (numPulses < N_PULSES_MAX)
				numPulses += N_PULSES_INCREMENT;
				PrintValues();
				Gpio_timing();
		break;

		case DEC_PULSE:	
			if (numPulses > N_PULSES_MIN)
				numPulses -= N_PULSES_INCREMENT;
				PrintValues();
				Gpio_timing();
		break;

		case INC_ON_TIME:	
			if (onTime < ON_TIME_MAX)
				onTime += ON_TIME_INCREMENT;
				PrintValues();
				Gpio_timing();
		break;

		case DEC_ON_TIME:	
			if (onTime > ON_TIME_MIN)
				onTime -= ON_TIME_INCREMENT;
				PrintValues();
				Gpio_timing();
		break;

		case INC_OFF_TIME:	
			if (offTime < OFF_TIME_MAX)
				offTime += OFF_TIME_INCREMENT;
				PrintValues();
				Gpio_timing();
		break;

		case DEC_OFF_TIME:	
			if (offTime > OFF_TIME_MIN)
				offTime -= OFF_TIME_INCREMENT;
				PrintValues();
				Gpio_timing();
		break;

		case INC_BACKOFF_TIME:	
			if (backoffTime < BACKOFF_TIME_MAX)
				backoffTime += BACKOFF_TIME_INCREMENT;
				PrintValues();
				Gpio_timing();
		break;

		case DEC_BACKOFF_TIME:	
			if (backoffTime > BACKOFF_TIME_MIN)
				backoffTime -= BACKOFF_TIME_INCREMENT;
				PrintValues();
				Gpio_timing();
		break;

		case REFRESH_SCN:
			PrintScreen();
			PrintValues();
		break;

		}	// end case

	} /*end while*/

	endwin();				/* End curses mode */
	Gpio_quit();		// Stop the pulses & leave it tidy

	return 0;
}



