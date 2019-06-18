#include <kilolib.h>

#define SEED_ID 42 // kilo_uid for the seed
#define GRADIENT_MAX 255 // Max gradient. Corresponds with bot that hasn't recieved message
#define SECOND 32 // ~32 clock cycles per second


uint32_t last_changed = 0; // To reset clock cycle
uint8_t new_message = 0;    // New message flag
message_t rcvd_message; // Message struct for recieved message. Defined on kilobotics
uint8_t my_gradient_value;  // gradient of the bot
message_t transmitting; // Message struct for transmitted message. Defined on kilobotics
uint8_t rcvd_gradient = GRADIENT_MAX;   // Initialize recieved gradient as max value

void message_rx(message_t *msg, distance_measurement_t *dist) {
    rcvd_gradient = msg->data[0]; // Recieved gradient
    new_message = 1;    // New message flag
}

message_t *message_tx() {
    return &transmitting;
}


void setup() {
    last_changed = kilo_ticks;  // Set initial clock cycle

    // Set gradient of 0 for the seed, else 255
    if (kilo_uid == SEED_ID) {
        my_gradient_value = 0;
    } else {
        my_gradient_value = GRADIENT_MAX;
    }
    transmitting.type = NORMAL;
    transmitting.data[0] = my_gradient_value;   // Transmit starting gradient (0 if seed, else 255 )
    transmitting.crc = message_crc(&transmitting);  // You must set the CRC value before transmission
}

void loop() {
    if (kilo_uid != SEED_ID) {

        // Reset gradient if no message recieved from closer bot in last 2 seconds
        if (kilo_ticks > (last_changed + 2*SECOND)) {
            last_changed = kilo_ticks;
            my_gradient_value = GRADIENT_MAX;
        }
        if (new_message == 1) {

            // if new message and its from the next closest node, reset timer
            if (my_gradient_value == rcvd_gradient + 1)  {
                last_changed = kilo_ticks;
            }

            // If recieved gradient is my_gradient_value - 2 or lower
            // set my_gradient and transmit data. Set CRC value before transmission.
            if (my_gradient_value > (rcvd_gradient + 1)) {
                last_changed = kilo_ticks;
                my_gradient_value = (rcvd_gradient + 1);
                transmitting.data[0] = my_gradient_value;
                transmitting.crc = message_crc(&transmitting);
            }

            new_message = 0; // reset new message flag
        }
    }

    // Set gradient color
    if (my_gradient_value == 0) {   // Seed = white
        set_color(RGB(1, 1, 1));

    } else if (my_gradient_value == GRADIENT_MAX) { // Max gradient = Yellow
        set_color(RGB(1, 1, 0));

    } else if (my_gradient_value % 5 == 1) {    // Red
        set_color(RGB(1, 0, 0));

    } else if (my_gradient_value % 5 == 2) {    // Green
        set_color(RGB(0, 1, 0));

    } else if (my_gradient_value % 5 == 3) {    // Blue
        set_color(RGB(0, 0, 1));

    } else if (my_gradient_value % 5 == 4) {    // Magenta
        set_color(RGB(1, 0, 1));

    } else if (my_gradient_value % 5 == 0) {    // Cyan
        set_color(RGB(0, 1, 1));
    }
}

int main() {
    // initialize hardware
    kilo_init();
    kilo_message_tx = message_tx;
    kilo_message_rx = message_rx;
    kilo_start(setup, loop);

    return 0;
}
