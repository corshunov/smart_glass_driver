//#define LOG
#define ALWAYS_ENABLE

#define A_PIN 3
#define B_PIN 4
#define EN_PIN 5

#define PERC_LOW 0.0

float ac_period;
float ac_pulse_ms;
float ac_pause_ms;

int ac_frequency = 111;

float perc_on_border_1 = 0.05;
float perc_on_border_2 = 0.20;

float step_on_start = 0.02;
float step_on_middle = 0.001;
float step_on_end = 0.002;

float perc_off_border_1 = 0.15;
float perc_off_border_2 = 0.30;

float step_off_start = 0.0015;
float step_off_middle = 0.004;
float step_off_end = 0.004;

float perc_high = 0.49;

bool state = 0;

void setup() {
  pinMode(A_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);

  ac_period = 1. / ac_frequency;

  Serial.begin(115200);

  Serial.println("Ready.");

  #ifdef ALWAYS_ENABLE
  digitalWrite(EN_PIN, HIGH);
  Serial.println("Enable pin is always HIGH.");
  #endif
}

void loop() {
  char cmd;
  float perc;

  if (Serial.available() > 0) {
    cmd = Serial.read();

    #ifdef LOG
    Serial.println(cmd);
    #endif

    if (cmd == '1' && !state) {
      Serial.print("ON");

      #ifdef LOG
      int time_before = millis();
      #endif

      turnOn();

      #ifdef LOG
      int time_diff = millis() - time_before;
      Serial.print(" [changed in ");
      Serial.print(time_diff);
      Serial.print(" ms]");
      #endif

      Serial.println("");

      state = !state;

    } else if (cmd == '0' && state) {
      Serial.print("OFF");

      #ifdef LOG
      int time_before = millis();
      #endif

      turnOff();

      #ifdef LOG
      int time_diff = millis() - time_before;
      Serial.print(" [changed in ");
      Serial.print(time_diff);
      Serial.print(" ms]");
      #endif

      Serial.println("");

      state = !state;
    }
  }

  if (state) {
    perc = perc_high;
  } else {
    perc = PERC_LOW;
  }

  cycle(perc);
}

void turnOn() {
  float step;
  
  for (float i = PERC_LOW; i <= perc_high;) {
    cycle(i);

    step = get_on_step(i);
    i = i + step;
  }
}

void turnOff() {
  float step;
  
  for (float i = perc_high; i >= PERC_LOW;) {
    cycle(i);
    
    step = get_off_step(i);
    i = i - step;
  }
}

void cycle(float percent) {
  ac_pulse_ms = ac_period * percent * 1000000;
  ac_pause_ms = ac_period * (0.5 - percent) * 1000000;

  if (ac_pulse_ms == 0) {
    digitalWrite(A_PIN, LOW);
    digitalWrite(B_PIN, LOW);
    #ifndef ALWAYS_ENABLE
    digitalWrite(EN_PIN, LOW);
    #endif
    return;
  }

  digitalWrite(A_PIN, HIGH);
  digitalWrite(B_PIN, LOW);
  #ifndef ALWAYS_ENABLE
  digitalWrite(EN_PIN, HIGH);
  #endif
  delayMicroseconds(ac_pulse_ms);
  #ifndef ALWAYS_ENABLE
  digitalWrite(EN_PIN, LOW);
  #endif
  digitalWrite(A_PIN, LOW);
  delayMicroseconds(ac_pause_ms);

  digitalWrite(B_PIN, HIGH);
  digitalWrite(A_PIN, LOW);
  #ifndef ALWAYS_ENABLE
  digitalWrite(EN_PIN, HIGH);
  #endif
  delayMicroseconds(ac_pulse_ms);
  #ifndef ALWAYS_ENABLE
  digitalWrite(EN_PIN, LOW);
  #endif
  digitalWrite(B_PIN, LOW);
  delayMicroseconds(ac_pause_ms);
}

float get_on_step(float percent) {
  if (percent <= perc_on_border_1) {
    return step_on_start;
  } else if (percent <= perc_on_border_2) {
    return step_on_middle;
  } else {
    return step_on_end;
  }
}

float get_off_step(float percent) {
  if (percent <= perc_off_border_1) {
    return step_off_start;
  } else if (percent <= perc_off_border_2) {
    return step_off_middle;
  } else {
    return step_off_end;
  }
}
