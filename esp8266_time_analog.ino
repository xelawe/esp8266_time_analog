#define serdebug
#ifdef serdebug
#define DebugPrint(...) {  Serial.print(__VA_ARGS__); }
#define DebugPrintln(...) {  Serial.println(__VA_ARGS__); }
#else
#define DebugPrint(...) { }
#define DebugPrintln(...) { }
#endif

#include "tools_wifiman.h"
#include "ClockAnalog.h"
#include "pulse.h"
#include "ntptool.h"

ClockAnalog ClockA(12, 13, 45);

void do_status() {
  time_t sy_time = nowl();

  DebugPrintln(" ");
  DebugPrintln("Sys   : " + String(hour(sy_time)) + ":" + String(minute(sy_time)) + ":" + String(second(sy_time)) );
  DebugPrintln("Clock : " + String(hour(time_tick)) + ":" + String(minute(time_tick)) + ":" + String(second(time_tick)) );
  DebugPrintln(" ");
}

void tick( ) {
  //adjust_clock( nowl() );
  int diff_sec = ClockA.adjust_time( nowl() );
  DebugPrintln("Diff " + String(diff_sec) + " seconds" );
  //do_step_sec();
  ClockA.step_sec();
}

void setup() {
#ifdef serdebug
  Serial.begin(115200);
#endif

  DebugPrintln("__DATE__ __TIME__ __FILE__");


  //pulse_init();
  ClockA.init();

  wifi_init();

  ntp_init();

  do_status();

  //clock_time_init(nowl());
  ClockA.init_time(nowl());

  do_status();

  //Alarm.timerRepeat(1,  do_step_sec);
  Alarm.timerRepeat(1,  tick);
  Alarm.timerRepeat(60, do_status);

}

void loop() {

  check_time();

  Alarm.delay(10);
}

