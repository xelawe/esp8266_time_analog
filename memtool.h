#include <Time.h> //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h> // https://github.com/PaulStoffregen/Time

// CRC function used to ensure data validity
uint32_t calculateCRC32(const uint8_t *data, size_t length);

// helper function to dump memory contents as hex
void printMemory();

// Structure which will be stored in RTC memory.
// First field is CRC32, which is calculated based on the
// rest of structure contents.
// Any fields can go after CRC32.
// We use byte array as an example.
struct {
  uint32_t crc32;
  time_t mem_time;
} rtcData;

boolean is_mem_valid() {
  // Read struct from RTC memory
  if (ESP.rtcUserMemoryRead(0, (uint32_t*) &rtcData, sizeof(rtcData))) {
    DebugPrintln("Read: ");
    printMemory();
    DebugPrintln();
    uint32_t crcOfData = calculateCRC32(((uint8_t*) &rtcData) + 4, sizeof(rtcData) - 4);
    DebugPrint("CRC32 of data: ");
    DebugPrintln(crcOfData, HEX);
    DebugPrint("CRC32 read from RTC: ");
    DebugPrintln(rtcData.crc32, HEX);
    if (crcOfData != rtcData.crc32) {
      DebugPrintln("CRC32 in RTC memory doesn't match CRC32 of data. Data is probably invalid!");
      return false;
    }
    else {
      DebugPrintln("CRC32 check ok, data is probably valid.");
      return true;
    }
  }
}

void time_to_mem( time_t iv_time ) {

  // Generate new data set for the struct
  rtcData.mem_time = iv_time;

  // Update CRC32 of data
  rtcData.crc32 = calculateCRC32(((uint8_t*) &rtcData) + 4, sizeof(rtcData) - 4);
  // Write struct to RTC memory
  if (ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcData, sizeof(rtcData))) {
    //DebugPrintln("Write: ");
    //printMemory();
    //DebugPrintln();
  }

  //Serial.println("Going into deep sleep for 5 seconds");
  //ESP.deepSleep(5e6);
}


uint32_t calculateCRC32(const uint8_t *data, size_t length)
{
  uint32_t crc = 0xffffffff;
  while (length--) {
    uint8_t c = *data++;
    for (uint32_t i = 0x80; i > 0; i >>= 1) {
      bool bit = crc & 0x80000000;
      if (c & i) {
        bit = !bit;
      }
      crc <<= 1;
      if (bit) {
        crc ^= 0x04c11db7;
      }
    }
  }
  return crc;
}

void printMemory() {
  time_t mem_time = rtcData.mem_time;

  DebugPrintln();
  DebugPrintln("Mem   : " + String(hour(mem_time)) + ":" + String(minute(mem_time)) + ":" + String(second(mem_time)) );

  Serial.println();
}
