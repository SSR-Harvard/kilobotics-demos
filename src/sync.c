/* 6/18/19: Written by Gabriel Seymour in the 2019 Harvard SEAS REU
 * This program runs on the kilobots to sync up their flashes, although more
 * functionality can be added (such as motor spinup when flashing). A bot
 * recieves messages from its neighbors containing their location in the flash
 * cycle (period of 64). By adjusting the length of the bot's own flash cycle
 * from the average of its neighbors location in the cycle, we can shorten or
 * lengthen the cycle to sync with the average of its neighbors. After this process
 * occurs a few times, the bots time in the flash cycle will converge on a local
 * average. Note that with more bots, it will take longer to converge. Also, the more
 * spread out your bot cluster is, the more you will see "flash ripples" through the
 * group. This is due to the nature of local communication amongst the bots.
 */
#include <kilolib.h>

#define PERIOD 32*2        // Flash cycle of length 64 clock cycles
uint32_t last_reset;     // To reset clock cycle
uint32_t flash_reset;    // To keep the flash time consistent
uint32_t rcvd_time = 0;  // Neighbors' location in the flash cycle
message_t message;       // Message struct for sending message
message_t rcvd_message;  // Message struct for recieving message
int32_t sumSync = 0;     // The total of how far the neighbors' location is from local bot
int32_t numSync = 0;    // Number of recieved locations in current flash cycle
uint8_t new_message = 0; // New message flag
int32_t deltaT = 0;      // Local location in flash cycle - recieved location in flash cycle
float avgDeltaT;         // Avg(deltaT).

// Send message
message_t *message_tx() {
    message.data[0] = kilo_ticks - last_reset; // Position in flash cycle
    message.crc = message_crc(&message); // You must set the CRC value before transmission
    return &message;
}

// Recieve message from neighbor bot
void message_rx(message_t *msg, distance_measurement_t *dist) {
    rcvd_message = *msg;
    rcvd_time = msg->data[0]; // Neighbors location in the flash cycle
    new_message = 1; // Set new message flag
}

void setup() {
    // rand_hard()/2 makes the bots flash at a random starting point. Makes sync more impressive
    last_reset = kilo_ticks + rand_hard()/2;
    flash_reset = kilo_ticks; // To keep flash time consistent

}

void loop() {
    if (new_message == 1) {
        // Neighbors flash cycle position - my flash cycle position
        deltaT = rcvd_time - (kilo_ticks - last_reset);

        // The following shortens the time required to wait by a significant amount
        // if necessary. For example, if PERIOD = 64, my position in the
        // cycle = 10, and my neighbors = 60, then I could lengthen my next period
        // by 60 - 10 = 50, or I can just shorten it by |50 - 64| = 14 clock cycles
        // since 64 + (10 - 14) = 60. This reduces the wait time by a respectable amount.
        if (deltaT > 0.5*PERIOD)
            deltaT = deltaT - PERIOD;
        if (deltaT < -0.5*PERIOD) // Opposite position
            deltaT = deltaT + PERIOD;

        sumSync = sumSync + deltaT; // Aggregate total of deltaT in this flash cycle
        numSync = numSync + 1;      // Number of neighbor messages recieved in this flash cycle
    }

    // If we have gone through the entire flash cycle, flash
    if (kilo_ticks >= (last_reset + PERIOD)) {
        set_color(RGB(1, 1, 0)); // Bright red

        // Cast the average of deltaT as a float and add 0.5. This will effectively
        // round the number when avgDeltaT is added to an int
        avgDeltaT = (float)(sumSync/numSync) + 0.5;
        
	// Either lengthen or shorten the next clock cycle
        last_reset = kilo_ticks - avgDeltaT;
        flash_reset = kilo_ticks; // To keep the flash time consistent
        sumSync = 0; // Reset the aggregate total so the next flash cycle starts at 0
        numSync = 0; // Reset the number of messages so the next flash cycle starts at 0

    } else if (kilo_ticks > flash_reset + PERIOD/8){ // Flash for PERIOD/N clock cycles
        set_color(RGB(0, 0, 0));
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
