#include <kilolib.h>

uint32_t counter = 0;
void setup()
{
}

void loop()
{
    //set_motors(kilo_straight_left, kilo_straight_left);
    counter++;
    set_color(RGB(1, 0, 0));
    set_motors(150,150);
    delay(400);
    if (counter % 15 == 0) {
        set_motors(0, 0);
        set_color(RGB(1, 1, 1));
        delay(5000);
    } else {
        set_motors(kilo_straight_left, kilo_straight_left); 
    }
    delay(400);
}


int main()
{
    kilo_init();
    kilo_start(setup, loop);

    return 0;
}
