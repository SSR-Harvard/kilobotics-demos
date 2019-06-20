/* 6/20/19: Adapted from Melvin Gauci's code 'synchronized_phototaxis.c'
 * by Gabriel Seymour in the 2019 Harvard SEAS REU.
 * This program initially spins a kilobot for a random amount of time between
 * 0 and 2550 clock cycles. It then spins the bot until a light intensity dip
 * is found (caused by the charging tab). The bot will "walk" back and forth (left and right) around this
 * intensity dip, effectively walking towards the light source
 */
#include <kilolib.h>
#include <math.h>


static const uint16_t PERIOD = 128;		// Flash cycle of length 128 clock cycles
uint32_t last_reset = 0;				// To reset clock cycle
float sawTooth;							// A value from 0 to 1 to switch between walking directions
uint32_t ticks;							// Location in the current cycle (between 0 and PERIOD - 1)

void phototaxis(float clock);			// Function to find the dip in light
uint16_t get_averaged_ambient_light();	// Function to get the average light intensity of 500 samples


void setup()
{
	last_reset = kilo_ticks;			// Initial cycle start time
	// Introduce a random delay so the robots don't become instantly
	// synchronized by the run signal.
	set_color(RGB(1, 1, 1));	// Initially set color to red
	spinup_motors();
	set_motors(0, kilo_turn_right);		// Spin so not every bot is pointing in the same direction
	delay(10 * rand_hard());			// Delay by a random amount so the phototaxis happens sporadically
	set_color(RGB(0, 0, 0));			// Turn off light
}

void loop()
{
	ticks = kilo_ticks - last_reset; 	// Location in the cycle
	if (ticks >= PERIOD) { 				// If outside of the period
		last_reset = kilo_ticks; 		// Reset the period
	}
	sawTooth = ((float) (kilo_ticks - last_reset) / (PERIOD)); // Value between 0 and 1

	phototaxis(sawTooth);	// Determine walking direction
}

/* phototaxis() looks for dips in the light intensity. It then walks back and
 * forth around this light intensity. The graph below shows a depiction of
 * the light intensity versus viewing angle. The bot will turn left until it
 * hits T1, and then it will turn right until it hits T2. This repeats until the
 * program is halted or until the bots can't discern where the light is
 * Intensity 	| ---			  ---
 *				|	 \           /
 *				|	  \         /
 *      		|	   \_______/
 * 				|----T1----------T2------
 *					Theta
 */
void phototaxis(float clock)
{
	typedef enum {t1, t2} clock_state_t;	// To select turning direction

	typedef enum {lt, rt} direction_t;		// To tell the bot to turn
	static clock_state_t clock_state = t1;	// Initial turning direction
	static direction_t direction = rt;		// Will be switched to left
	static direction_t past_direction = rt;	// So we don't spinup the motors unnecessarily
	static uint16_t lo_threshold = 1024;	// Light measure from 0 to 1023
	static uint16_t hi_threshold = 0;		// Light measure from 0 to 1023

	uint16_t current_light = get_averaged_ambient_light();	// Average light measurement of 500 measurements
	if (current_light < lo_threshold)		// Update threshold if we're below the min
	{
		lo_threshold = round(0.95 * current_light);
		hi_threshold = round(1.05 * current_light);
	}
	else if (current_light > hi_threshold)
	{
		// Update threshold if we're above the hi_threshold
		lo_threshold = round(0.95 * current_light);
		hi_threshold = round(1.05 * current_light);

		direction = 1 - direction;			// change direction
		set_motors(0, 0);					// stop moving
	}

	if (clock_state == t1) {				// State one
		if (clock > 0.5) {					// Should run for ~clock/2 ticks
			clock_state = t2;				// Next cycle state
			set_color(RGB(0, 0, 1));		// Blue

			// If we're turning right and we weren't already turning right
			if (direction == rt && past_direction != rt) {
				spinup_motors();			// Start the motors
				set_motors(0, kilo_turn_right);	// Turn right
				past_direction = rt;		// Update past_direction
			}
		}
	} else if (clock_state == t2) {			// State two
		if (clock <= 0.5) {					// Should run for ~clock/2 ticks
			clock_state = t1;				// Next cycle state

			set_color(RGB(1, 0, 0));		// Red

			// If we're turning left and we weren't already turning left
			if (direction == lt && past_direction != lt) {
				spinup_motors();			// Start the motors
				set_motors(kilo_turn_left, 0);	// Turn left
				past_direction = lt;		// Update past_direction
			}
		}
	}
}

// get_averaged_ambient_light() gets the average light measurement of 500 samples
uint16_t get_averaged_ambient_light()
{
	static const uint16_t NUM_SAMPLES = 500;	// Semi-arbitrary number. 300 is also good

	uint32_t sum = 0;						// Aggregate total of all 500 light measurements
	uint16_t sample_counter = 0;			// Number of measurements

	while (sample_counter < NUM_SAMPLES)
	{

		// Returns a number between 0 and 1023 if measurement achieved. -1 otherwise
		int16_t sample = get_ambientlight();
		if (sample != -1)					// If measurement achieved
		{
			sum += sample;					// Add to the aggregate total
			sample_counter++;				// Increment number of samples
		}
	}

	return (sum / NUM_SAMPLES);				// Agg. total / number of meas = average
}


int main()
{
	kilo_init();
	kilo_start(setup, loop);

	return 0;
}
