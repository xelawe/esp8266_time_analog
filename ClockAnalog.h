// Library to control analog clock with pulse motor

#ifndef ClockAnalog_h
#define ClockAnalog_h

#include "Arduino.h"

#include <Time.h> //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>

class ClockAnalog
{
  public:
    ClockAnalog(int iv_PINMOT1, int iv_PINMOT2, int iv_pulsewidth);
    void init();
    void init_time( time_t iv_time );
    int  adjust_time( time_t iv_time );
    void step();
    void step_sec(int iv_sec);
    void step_sec();
    void step_min(int iv_min);
    time_t time();

  private:
    int _mv_PINMOT1 = 12;
    int _mv_PINMOT2 = 13;

    int _mv_pulsewidth = 45;

    boolean _mv_pulse1 = true;
    boolean _mv_no_pulse = false;
    time_t _mv_clock_time;
    int    _mv_diff_sec;

    void _pulse( int iv_pin );

};

#endif
