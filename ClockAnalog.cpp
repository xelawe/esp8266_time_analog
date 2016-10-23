#include "Arduino.h"

#include "ClockAnalog.h"


ClockAnalog::ClockAnalog(int iv_PINMOT1, int iv_PINMOT2, int iv_pulsewidth)
{

  _mv_PINMOT1 = iv_PINMOT1;
  _mv_PINMOT2 = iv_PINMOT2;

  _mv_pulsewidth = iv_pulsewidth;

}

void ClockAnalog::init( ) {
  pinMode(_mv_PINMOT1, OUTPUT);
  pinMode(_mv_PINMOT2, OUTPUT);

  digitalWrite(_mv_PINMOT1, LOW);
  digitalWrite(_mv_PINMOT2, LOW);
}

void ClockAnalog::init_time( time_t iv_time ){

  // on first adjust we assume, that the clock is set to the right hour and we just have
  // to adjust minutes an seconds
  static tmElements_t tm;

  breakTime(iv_time, tm);

  tm.Minute = 0;
  tm.Second = 0;

  _mv_clock_time = makeTime(tm);

}

void ClockAnalog::step( ) {
  if ( _mv_no_pulse == true ) {
    // no pulses allowed, leave
    return;
  }

  if ( _mv_pulse1 == true ) {

    _pulse( _mv_PINMOT1 );
    _mv_pulse1 = false;

  } else {

    _pulse( _mv_PINMOT2 );
    _mv_pulse1 = true;
  }

  _mv_clock_time++;
  //  time_tick++;
  //  DebugPrint(".");
}

void ClockAnalog::_pulse( int iv_pin ) {
  digitalWrite(iv_pin, HIGH);
  delay(_mv_pulsewidth);
  digitalWrite(iv_pin, LOW);
}
