#define serdebug
#ifdef serdebug
#define DebugPrint(...) {  Serial.print(__VA_ARGS__); }
#define DebugPrintln(...) {  Serial.println(__VA_ARGS__); }
#else
#define DebugPrint(...) { }
#define DebugPrintln(...) { }
#endif

#include "tools_wifiman.h"
#include "ntptool.h"
#include "pulse.h"

#define btnpin 4
#define ledpinbl 13
#define ledpinrt 15
#define ledpingn 12
#define LDRPin (A0)
int LDRValue;

void do_status() {
  time_t sy_time = now();

  DebugPrintln(" ");
  DebugPrintln("Sys  : " + String(hour(sy_time)) + ":" + String(minute(sy_time)) + ":" + String(second(sy_time)) );
  DebugPrintln("Tick : " + String(hour(time_tick)) + ":" + String(minute(time_tick)) + ":" + String(second(time_tick)) );
  DebugPrintln(" ");
}

void setup() {
#ifdef serdebug
  Serial.begin(115200);
#endif

  pulse_init();

  wifi_init();

  ntp_init();

  do_status();

  time_t time_sync = now();
  DebugPrintln("Sync " + String(minute(time_sync)) + " minutes" );
  do_step_min(minute(time_sync));

  time_t time_diff = now() - time_sync;
  if ( minute(time_diff) >= 1) {
    DebugPrintln("Sync " + String(minute(time_diff)) + " minutes" );
    do_step_min(minute(time_diff));
  }

  time_tick = now();
  do_status();

  Alarm.timerRepeat(1,  do_step_sec);
  Alarm.timerRepeat(60, do_status);

}

void loop() {

  check_time();

  adjust_clock();

  Alarm.delay(10);
}
