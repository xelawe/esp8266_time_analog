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


#define btnpin 4
#define ledpinbl 13
#define ledpinrt 15
#define ledpingn 12
#define LDRPin (A0)
int LDRValue;

ClockAnalog ClockA(12, 13, 45);

void do_status() {
  time_t sy_time = nowl();

  DebugPrintln(" ");
  DebugPrintln("Sys   : " + String(hour(sy_time)) + ":" + String(minute(sy_time)) + ":" + String(second(sy_time)) );
  DebugPrintln("Clock : " + String(hour(time_tick)) + ":" + String(minute(time_tick)) + ":" + String(second(time_tick)) );
  DebugPrintln(" ");
}

void setup() {
#ifdef serdebug
  Serial.begin(115200);
#endif


  pulse_init();
  //ClockA.init();
  
  wifi_init();

  ntp_init();

  do_status();

  clock_time_init();

  do_status();

  //Alarm.timerRepeat(1,  do_step_sec);
  Alarm.timerRepeat(1,  tick);
  Alarm.timerRepeat(60, do_status);

}

void loop() {

  check_time();

  Alarm.delay(10);
}

