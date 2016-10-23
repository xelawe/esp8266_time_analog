
#include <Time.h> //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>

#define PINMOT1 12
#define PINMOT2 13

#define pulsewidth 45

boolean pulse1 = true;
boolean gv_no_pulse = false;
time_t time_tick;

int gv_diff_sec;

void do_pulse( int iv_pin ) {
  digitalWrite(iv_pin, HIGH);
  delay(pulsewidth);
  digitalWrite(iv_pin, LOW);
}

void do_step( ) {

  if ( gv_no_pulse == true ) {
    // no pulses allowed, leave
    return;
  }


  if ( pulse1 == true ) {

    do_pulse( PINMOT1 );
    pulse1 = false;

  } else {

    do_pulse( PINMOT2 );
    pulse1 = true;
  }

  time_tick++;
  DebugPrint(".");
}

void do_step_sec(int iv_sec) {

  for (int i = 0; i < iv_sec; i++) {
    do_step( );
    delay(20);
  }
}

void do_step_sec() {
  int lv_sec;

  if ( gv_diff_sec > 0 ) {
    lv_sec = gv_diff_sec;
    gv_diff_sec = 0;
  } else {
    lv_sec = 1;
  }
  
  do_step_sec(lv_sec);

}

void do_step_min(int iv_min) {
  // go one fast round
  for (int j = 0; j < iv_min; j++) {
    do_step_sec(60);
  }
}


void adjust_clock(time_t iv_time) {
  time_t sys_time = iv_time;
  time_t clock_time = time_tick;
  time_t diff_time;
  int diff_sec;

  // time set?
//  if ( timeStatus() == timeNotSet ) {
//    return;
//  }

  // if set, but not synced, we adjust to system time anyway

  if (clock_time == sys_time) {
    return;
  }

  if (clock_time < sys_time) {
    // Clock behind system time

    // pulses are allowed
    gv_no_pulse = false;

    // calculate difference
    diff_time = sys_time - clock_time;
    //DebugPrint("Diff : " + String(year(diff_time)) + "." + String(month(diff_time)) + "." + String(day(diff_time)) );
    //DebugPrintln(", " + String(hour(diff_time)) + ":" + String(minute(diff_time)) + ":" + String(second(diff_time)) );

    diff_sec = second(diff_time);

    diff_sec = diff_time;
    if ( diff_sec > 1) {
      DebugPrintln("Diff " + String(diff_sec) + " seconds" );
      gv_diff_sec = diff_sec;
    }

  } else {
    // Clock in front system time
    // so we have to wait and don't do any pulses
    gv_no_pulse = true;
  }


}


void clock_time_init(time_t iv_time) {
  // on first adjust we assume, that the clock is set to the right hour and we just have
  // to adjust minutes an seconds

  static tmElements_t tm;

  breakTime(iv_time, tm);

  tm.Minute = 0;
  tm.Second = 0;

  time_tick = makeTime(tm);

}


void pulse_init() {
  pinMode(PINMOT1, OUTPUT);
  pinMode(PINMOT2, OUTPUT);

  digitalWrite(PINMOT1, LOW);
  digitalWrite(PINMOT2, LOW);
}

