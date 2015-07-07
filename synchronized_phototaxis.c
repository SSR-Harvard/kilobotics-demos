#include <kilolib.h>
#include <math.h>

typedef enum {false, true} bool;

// Nominal period with which to blink. This will be the period of every robot
// once the swarm has synchronized.
static const uint16_t PERIOD = 256;

message_t tx_message;
message_t rx_message;

bool global_new_message_flag = false;

float synchronization(bool new_message);
int32_t compute_rx_reset_time_adjustment(uint32_t last_reset);
void phototaxis(float clock);
uint16_t get_averaged_ambient_light();

void setup()
{
	tx_message.type = NORMAL;

	// Introduce a random delay so the robots don't become instantly
	// synchronized by the run signal.
	set_color(RGB(1, 1, 1));
	delay(10 * rand_hard());
	set_color(RGB(0, 0, 0));
}

void loop()
{
	// Wall robots do nothing.
	if (kilo_uid == 0)
	{
	    set_color(RGB(0, 0, 0));
		return;
	}

	bool local_new_message_flag = 0;
	if (global_new_message_flag)
	{
		global_new_message_flag = false;
		local_new_message_flag = true;
	}

	float clock = synchronization(local_new_message_flag);
	phototaxis(clock);

	local_new_message_flag = false;
}

void phototaxis(float clock)
{
	typedef enum {lo_on, lo_off, hi_on, hi_off} clock_state_t;
	typedef enum {lt, rt} direction_t;

	static clock_state_t clock_state = hi_off;
	static direction_t direction = rt;

	static uint16_t lo_threshold = 1024;
	static uint16_t hi_threshold = 0;

	uint16_t current_light = get_averaged_ambient_light();
	if (current_light < lo_threshold)
	{
		lo_threshold = round(0.95 * current_light);
		hi_threshold = round(1.05 * current_light);
	}
	else if (current_light > hi_threshold)
	{
		lo_threshold = round(0.95 * current_light);
		hi_threshold = round(1.05 * current_light);

		direction = 1 - direction;
		set_motors(0, 0);
	}

	switch (clock_state)
	{
	case lo_on:
		if (clock > 0.4)
		{
			clock_state = lo_off;

			set_color(RGB(0, 0, 0));
			set_motors(0, 0);
		}
		break;
	case lo_off:
		if (clock > 0.5)
		{
			clock_state = hi_on;

			set_color(RGB(0, 0, 1));
			if (direction == rt)
			{
				spinup_motors();
				set_motors(0, kilo_turn_right);
			}
			else
			{
				set_motors(0, 0);
			}
		}
		break;
	case hi_on:
		if (clock > 0.9)
		{
			clock_state = hi_off;

			set_color(RGB(0, 0, 0));
			set_motors(0, 0);
		}
		break;
	case hi_off:
		if (clock < 0.4)
		{
			clock_state = lo_on;

			set_color(RGB(1, 0, 0));
			if (direction == lt)
			{
				spinup_motors();
				set_motors(kilo_turn_left, 0);
			}
			else
			{
				set_motors(0, 0);
			}

		}
		break;
	}
}

uint16_t get_averaged_ambient_light()
{
	static const uint16_t NUM_SAMPLES = 500;

	uint32_t sum = 0;
	uint16_t sample_counter = 0;

	while (sample_counter < NUM_SAMPLES)
	{
		int16_t sample = get_ambientlight();
		if (sample != -1)
		{
			sum += sample;
			sample_counter++;
		}
	}

	return (uint16_t) (sum / NUM_SAMPLES);
}

float synchronization(bool new_message)
{
	static uint32_t last_reset = 0;
	static uint32_t reset_time = 0;
	static long double reset_time_adjustment = 0;
	static uint16_t number_of_messages = 0;

	if ((kilo_ticks >= reset_time) || ((kilo_ticks - last_reset) > UINT16_MAX))
	{
		// 2 is an experimentally-chosen factor. + 1 to avoid division by 0.
		reset_time_adjustment /= ((2 * number_of_messages) + 1);

		last_reset = kilo_ticks;
		reset_time = kilo_ticks + PERIOD + round(reset_time_adjustment);

		reset_time_adjustment = 0;
		number_of_messages = 0;
	}

	if (new_message)
	{
		number_of_messages++;
		reset_time_adjustment += compute_rx_reset_time_adjustment(last_reset);
	}

	uint16_t ticks_until_reset = kilo_ticks - last_reset;
	tx_message.data[1] = ((ticks_until_reset >> 8) & 0x00FF);
	tx_message.data[0] = (ticks_until_reset & 0x00FF);

	// Creates a sawtooth wave going from 0 to 1 during each period.
	return ((float) (kilo_ticks - last_reset)) / (reset_time - last_reset);
}

int32_t compute_rx_reset_time_adjustment(uint32_t last_reset)
{
	uint16_t my_timer = kilo_ticks - last_reset;
	uint16_t rx_timer = (((uint16_t) rx_message.data[1]) << 8)
			| ((uint16_t) (rx_message.data[0]));

	int32_t timer_discrepancy = rx_timer - my_timer;
	uint16_t timer_discrepancy_abs = fabs(timer_discrepancy);
	int8_t timer_discrepancy_sgn = copysignf(1.0, timer_discrepancy);

	int32_t rx_reset_time_adjustment = 0;

	if (timer_discrepancy_abs < (PERIOD / 2))
	{
		rx_reset_time_adjustment = - timer_discrepancy;
	}
	else
	{
		rx_reset_time_adjustment = timer_discrepancy_sgn
				* ((PERIOD - timer_discrepancy_abs) % PERIOD);
	}

	return rx_reset_time_adjustment;
}

message_t *message_tx()
{
	tx_message.crc = message_crc(&tx_message);
	return &tx_message;
}

void message_rx(message_t *m, distance_measurement_t *d)
{
	global_new_message_flag = 1;
	rx_message = *m;
}

int main()
{
	kilo_init();

	kilo_message_rx = message_rx;
	kilo_message_tx = message_tx;

	kilo_start(setup, loop);

	return 0;
}
