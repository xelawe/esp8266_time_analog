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

void ClockAnalog::init_time( time_t iv_time ) {

  // on first adjust we assume, that the clock is set to the right hour and we just have
  // to adjust minutes an seconds
  static tmElements_t tm;

  breakTime(iv_time, tm);

  tm.Minute = 0;
  tm.Second = 0;

  _mv_clock_time = makeTime(tm);

}

int ClockAnalog::adjust_time(time_t iv_time) {
  time_t sys_time = iv_time;
  time_t clock_time = _mv_clock_time;
  time_t diff_time;
  int lv_diff_sec;

  // pulses are allowed
  _mv_no_pulse = false;
  _mv_diff_sec = 0;

  if (clock_time == sys_time) {
    return _mv_diff_sec;
  }

  if (clock_time < sys_time) {
    // Clock behind system time

    // calculate difference
    diff_time = sys_time - clock_time;
    //DebugPrint("Diff : " + String(year(diff_time)) + "." + String(month(diff_time)) + "." + String(day(diff_time)) );
    //DebugPrintln(", " + String(hour(diff_time)) + ":" + String(minute(diff_time)) + ":" + String(second(diff_time)) );

    lv_diff_sec = diff_time;
    if ( lv_diff_sec > 1) {
      _mv_diff_sec = lv_diff_sec;
    }

  } else {
    // Clock in front system time
    // so we have to wait and don't do any pulses
    _mv_no_pulse = true;
  }

  return _mv_diff_sec;
}

void ClockAnalog::step_sec(int iv_sec) {

  for (int i = 0; i < iv_sec; i++) {
    step( );
    delay(20);
  }
}

void ClockAnalog::step_sec() {
  int lv_sec;

  if ( _mv_diff_sec > 0 ) {
    lv_sec = _mv_diff_sec;
    _mv_diff_sec = 0;
  } else {
    lv_sec = 1;
  }

  step_sec(lv_sec);

}

void ClockAnalog::step_min(int iv_min) {
  // go one fast round
  for (int j = 0; j < iv_min; j++) {
    step_sec(60);
  }
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
  
  //  DebugPrint(".");
}

void ClockAnalog::_pulse( int iv_pin ) {
  digitalWrite(iv_pin, HIGH);
  delay(_mv_pulsewidth);
  digitalWrite(iv_pin, LOW);
}

time_t ClockAnalog::time( ) {

  return _mv_clock_time;

}
