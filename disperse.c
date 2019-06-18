/* 6/18/19: Written by Gabriel Seymour in the 2019 Harvard SEAS REU.
 * If bot recieves a signal from another bot, it will randomly choose a motion.
 * The possible motions are move forward, turn left, and turn right.
 * While moving, the bot will turn green if moving forward, blue if turning right,
 * and red if turning left. If stopped (i.e. out of range of other bots), the
 * bot will be white. The bots randomly move until they are out of reach from
 * each other bots.
 */

#include <kilolib.h>

// preprocessor definitions for the different motions
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

#define PERIOD 64    // Define how often a bot should change direction

int cur_motion = 0;         // Keep track of previous motion
uint32_t last_changed = 0;  // Keep track of period
int new_message = 0;        // New message flag
int random_number = 0;      // Keep some space to define random number
int dice = 0;               // Keep some space to define random dice number
message_t transmit_msg; // message struct for sending message
message_t rcvd_message; // message struct for recieving message

// Set motion as defined by a random number
void set_motion(int new_motion) {
    if (cur_motion != new_motion) { // Only necessary if going a different direction
        spinup_motors(255, 255);    // Must spinup motors if changing direction

        if (new_motion == STOP) {   // Dont move
            set_motors(0, 0);

        } else if (new_motion == FORWARD) { // Go forward
            set_motors(kilo_straight_left, kilo_straight_right);

        } else if (new_motion == LEFT) { // Go left
            set_motors(kilo_turn_left, 0);

        } else {    // Go right
            set_motors(0, kilo_turn_right);
        }
        cur_motion = new_motion; // Set current motion

    }
}

// Send message
message_t *message_tx() {
    return &transmit_msg;
}

// Recieve message
void message_rx(message_t *msg, distance_measurement_t *dist) {
    rcvd_message = *msg;
    new_message = 1; // Set recieved message flag
}



void setup() {
    transmit_msg.type = NORMAL; // NORMAL is standard
    transmit_msg.data[0] = 0;   // Just send a 0... arbitrary
    transmit_msg.crc = message_crc(&transmit_msg);  // Must set message_crc before sending
    last_changed = kilo_ticks;  // Set first cycle starting point
}

void loop() {
    // If we've recieved a new message and we've waited one cycle
    if (new_message == 1 && kilo_ticks > (last_changed + PERIOD)) {
        last_changed = kilo_ticks;  // Reset cycle
        new_message = 0;            // Reset new message flag

        random_number = rand_hard();    // rand_hard() defined on kilobotics.com
        dice = (random_number % 4);     // Give one of 4 random numbers (0, 1, 2, 3)
        if ((dice == 0) || (dice == 1)) {   // 1/2 chance of going forward
            set_motion(FORWARD);
            set_color(RGB(0, 1, 0));    // Set green

        } else if (dice == 2) {         // 1/4 chance of going left
            set_motion(LEFT);
            set_color(RGB(1, 0, 0));    // Set red

        } else if (dice == 3) {         // 1/4 change of going right
            set_motion(RIGHT);
            set_color(RGB(0, 0, 1));    // Set blue

        } else {
            set_motion(STOP);
            set_color(RGB(0, 1, 1));    // If something was weird, set yellow and stop
        }

    // If we waited enough time and didn't get any messages, stop and turn white
    } else if (new_message == 0 && kilo_ticks > (last_changed + 64)) {
        set_motion(STOP);
        last_changed = kilo_ticks;  // Reset cycle... continuously looks for messages
        set_color(RGB(1, 1, 1));    // Turn white
        }
}

int main() {
    // initialize hardware
    kilo_init();
    kilo_message_rx = message_rx;
    kilo_message_tx = message_tx;
    kilo_start(setup, loop);

    return 0;
}
