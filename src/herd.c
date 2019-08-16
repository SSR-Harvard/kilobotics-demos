#include <kilolib.h>

uint32_t counter = 0; // Global counter

void setup()
{
}

void loop()
{
    
    ++counter;			// Increment
    set_color(RGB(1, 0, 0));	// Turn red
    set_motors(150,150);	// Found these motor values work well. Not too strong but enough to move them
    delay(400);			// Just spin for some time
    if (counter % 15 == 0) {	// Every 15 increments
        set_motors(0, 0);	// Turn the motors off
        set_color(RGB(1, 1, 1));// Turn the lights white
        delay(5000);		// Pause for some time
    } else {
        set_motors(kilo_straight_left, kilo_straight_left); // Let them walk a little bit
    }
    delay(400);			// Pause for more time
}


int main()
{
    kilo_init();
    kilo_start(setup, loop);

    return 0;
}
