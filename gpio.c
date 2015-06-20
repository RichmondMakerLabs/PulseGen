// gpio.c Operates in-out for pggui.c
#include <pigpiod_if.h>

#define PULSE_GPIO 23	// Pin 16, In1 of driver
#define PULSEOUT (1<<PULSE_GPIO)
#define PUMP_GPIO 24	// Pin 18, In2 of driver

extern int onTime;
extern int offTime;
extern int numPulses;
extern int backoffTime;
int Gpio_timing();
int Gpio_pulse(int state);
int Gpio_pump(int state);

// gpioPulse_t pulse[2*N_PULSES_MAX+2];
gpioPulse_t pulse[52];
int wave_id, p;

// Initialise once
int Gpio_init()
{
	// open socket connection to daemon on localhost, port 8888
	pigpio_start(NULL,NULL);

	set_mode(PULSE_GPIO, PI_OUTPUT);
	set_mode(PUMP_GPIO, PI_OUTPUT);

	gpio_write( PULSE_GPIO, 0);
	gpio_write( PUMP_GPIO, 0);
	
	Gpio_timing();	// Sets up the default waveform
	
	return 0;
}

int Gpio_quit()
{
	Gpio_pulse(0);
	Gpio_pump(0);
	pigpio_stop();
	return 0;
}

// turns pulses on or off
int Gpio_pulse(int state)
{
	if (state)	// turn on
		return (wave_send_repeat(wave_id));
	else		// turn off
	{
		wave_tx_stop();
		return (gpio_write( PULSE_GPIO, 0));
	}
}

// switches pump on or off, returns zero on success
int Gpio_pump(int state)
{
	return (gpio_write( PUMP_GPIO, state));
}

// sends wave parameters to pigpio daemon 
// returns -1 on error
int Gpio_timing()
{
	for (p=0; p<numPulses; p++)
	{
		pulse[2*p].gpioOn = 0;
		pulse[2*p].gpioOff = PULSEOUT;
		pulse[2*p].usDelay = offTime;
				
		pulse[2*p+1].gpioOn = PULSEOUT;
		pulse[2*p+1].gpioOff = 0;
		pulse[2*p+1].usDelay = onTime;
	}
	p = 2*numPulses;
	pulse[p].gpioOn = 0;
	pulse[p].gpioOff = PULSEOUT;
	pulse[p].usDelay = backoffTime - offTime;
		
	wave_clear();
	if ((wave_add_generic(2*numPulses+1,pulse)) != 2*numPulses+1)
		return -1;
	wave_id = wave_create();
	
	if (wave_id >= 0)
	{
		wave_send_repeat(wave_id);
		return 0;
	}
	else
	{
		// wave create failed
		return (-1);
	}
	

}
