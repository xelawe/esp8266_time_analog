// Library to control analog clock with pulse motor

#ifndef ClockAnalog_h
#define ClockAnalog_h

#include "Arduino.h"

class ClockAnalog
{
  public:
    ClockAnalog(int iv_PINMOT1, int iv_PINMOT2, int iv_pulsewidth);
    void init();
void init_time( time_t iv_time );
    void step();

  private:
    int _mv_PINMOT1 = 12;
    int _mv_PINMOT2 = 13;

    int _mv_pulsewidth = 45;

    boolean _mv_pulse1 = true;
    boolean _mv_no_pulse = false;
    time_t _mv_clock_time;

    void _pulse( int iv_pin );

};

#endif
