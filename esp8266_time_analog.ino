#define serdebug
#ifdef serdebug
#define DebugPrint(...) {  Serial.print(__VA_ARGS__); }
#define DebugPrintln(...) {  Serial.println(__VA_ARGS__); }
#else
#define DebugPrint(...) { }
#define DebugPrintln(...) { }
#endif

#include "tools_wifiman.h"
#include <ClockAnalog.h>
#include "ntptool.h"

ClockAnalog ClockA(12, 13, 45);

void do_status() {
  time_t sy_time = nowl();
  time_t clock_time = ClockA.time();

  DebugPrintln(" ");
  DebugPrintln("Sys   : " + String(hour(sy_time)) + ":" + String(minute(sy_time)) + ":" + String(second(sy_time)) );
  DebugPrintln("Clock : " + String(hour(clock_time)) + ":" + String(minute(clock_time)) + ":" + String(second(clock_time)) );
  DebugPrintln(" ");
}

void tick( ) {

  // time set?
  if ( timeStatus() != timeNotSet ) {
    // if set, but not synced, we adjust to system time anyway
    int diff_sec = ClockA.adjust_time( nowl() );
    if (diff_sec != 0) {
      DebugPrintln("Diff " + String(diff_sec) + " seconds" );
    }
  }

  ClockA.step_sec();
}

void setup() {
#ifdef serdebug
  Serial.begin(115200);
#endif

  DebugPrintln(" ");
  DebugPrint(__DATE__);
  DebugPrint(__TIME__);
  DebugPrintln(__FILE__);

  ClockA.init();

  wifi_init();

  ntp_init();

  do_status();

  ClockA.init_time(nowl());

  do_status();

  Alarm.timerRepeat(1,  tick);
  Alarm.timerRepeat(60, do_status);

}

void loop() {

  check_time();

  Alarm.delay(10);
}

