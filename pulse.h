
#define PINMOT1 12
#define PINMOT2 13

#define pulsewidth 45

boolean pulse1 = true;


void do_pulse( int iv_pin ) {
  digitalWrite(iv_pin, HIGH);
  delay(pulsewidth);
  digitalWrite(iv_pin, LOW);
}

void do_step( ) {

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

  lv_sec = 1 + gv_diff_sec;
  gv_diff_sec = 0;
  do_step_sec(lv_sec);

}

void do_step_min(int iv_min) {
  // go one fast round
  for (int j = 0; j < iv_min; j++) {
    do_step_sec(60);
  }
}

void pulse_init() {
  pinMode(PINMOT1, OUTPUT);
  pinMode(PINMOT2, OUTPUT);

  digitalWrite(PINMOT1, LOW);
  digitalWrite(PINMOT2, LOW);
}

