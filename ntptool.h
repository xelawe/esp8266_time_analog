
#include <Time.h> //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <Timezone.h>    //https://github.com/JChristensen/Timezone

#include <WiFiUdp.h>


/* Don't hardwire the IP address or we won't get the benefits of the pool.
    Lookup the IP address for the host name instead */
IPAddress timeServerIP; // time.nist.gov NTP server address
//const char* ntpServerName = "time.nist.gov";
const char* ntpServerName = "at.pool.ntp.org";

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
unsigned int localPort = 2390;      // local port to listen for UDP packets
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

int SyncInt = 300;
const int timeZone = 0;     // UTC

// Central European Time Zone (Vienna)
TimeChangeRule myDST = {"CEDT", Last, Sun, Mar, 2, 120};    //Daylight time = UTC - 4 hours
TimeChangeRule mySTD = {"CET", Last, Sun, Oct, 3,  60};     //Standard time = UTC - 5 hours
Timezone myTZ(myDST, mySTD);

TimeChangeRule *tcr;        //pointer to the time change rule, use to get TZ abbrev


/*-------- NTP code ----------*/
// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

time_t getNtpTime()
{

  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP);

  //DebugPrintln("Starting UDP");
  //Udp.begin(localPort);
  //Serial.print("Local port: ");
  //Serial.println(Udp.localPort());

  DebugPrintln("waiting for UDP sync");

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  DebugPrintln("Transmit NTP Request");

  sendNTPpacket(timeServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      DebugPrintln("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  DebugPrintln("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}


time_t nowl() {
  return  myTZ.toLocal(now(), &tcr);
}

void adjust_clock() {
  time_t sys_time = nowl();
  time_t clock_time = time_tick;
  time_t diff_time;
  int diff_sec;

  // time set?
  if ( timeStatus() == timeNotSet ) {
    return;
  }

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

void tick() {
  adjust_clock();
  do_step_sec();
}

void clock_time_init() {
  // on first adjust we assume, that the clock is set to the right hour and we just have
  // to adjust minutes an seconds
  time_t sys_time = nowl();
  static tmElements_t tm;

  breakTime(sys_time, tm);

  tm.Minute = 0;
  tm.Second = 0;

  time_tick = makeTime(tm);

}

void check_time() {
  if ( timeStatus() != timeSet ) {
    if (SyncInt != 5) {
      SyncInt = 5;
      setSyncInterval(SyncInt);
    }
  } else {
    if (SyncInt != 600) {
      SyncInt = 600;
      setSyncInterval(SyncInt);
    }
  }
}

void ntp_init() {


  DebugPrintln("Starting UDP");
  Udp.begin(localPort);

  setSyncProvider(getNtpTime);
  check_time();
  while (timeStatus() == timeNotSet) { // wait until the time is set by the sync provider
    delay(2000);
  }

  //reset_drift();

  DebugPrintln("done NTP");
}

