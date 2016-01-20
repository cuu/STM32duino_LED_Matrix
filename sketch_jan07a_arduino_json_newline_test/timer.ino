#include "timer.h"
#include "led.h"
#include "config.h"

HardwareTimer timer(4);

void timer_isr(void)
{
  led_scan(&cfg);
}

void stop_timer()
{
  timer.pause();
}
void restart_timer()
{
  timer.refresh();
  timer.resume();
}
void init_timer()
{
    timer.pause();

    // Set up period
    timer.setPeriod(500); // in microseconds

    // Set up an interrupt on channel 1
    timer.setChannel3Mode(TIMER_OUTPUT_COMPARE);
    timer.setCompare(TIMER_CH3, 1);  // Interrupt 1 count after each update
    timer.attachCompare1Interrupt( timer_isr );
    
    // Refresh the timer's count, prescale, and overflow
    timer.refresh();
    // Start the timer counting
    timer.resume();
}

