#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"

#include "pico_cppm/cppm_encoder.h"
#include "pico_cppm/cppm_decoder.h"

constexpr uint TEST_GPIO = 2;
constexpr double MIN_PERIOD_US = 1000;
constexpr double MAX_PERIOD_US = 2000;
constexpr double EXPECT_DELTA = 0.05;

void expectChannels(CPPMDecoder& decoder, const double expected[], const char* test_name) {
  for (int i = 0; i < 9; i++) {
    double actual = decoder.getChannelValue(i);
    if (fabs(actual - expected[i]) > EXPECT_DELTA) {
      printf("Failure in \"%s\": expected %f; actual %f\n", test_name, expected[i], actual);
    }
  }
}

int main() {
  stdio_init_all();

  sleep_ms(2500);
  printf("Begin test\n");

  CPPMEncoder encoder(TEST_GPIO, pio1);
  CPPMDecoder decoder(TEST_GPIO, pio0, 2500, 1000, 2000);
  decoder.startListening();
  encoder.startOutput();

  sleep_ms(50);
  expectChannels(decoder, (const double[]){0, 0, 0, 0, 0, 0, 0, 0, 0}, "initial values");

  encoder.setChannelValue(0, -1.5);
  encoder.setChannelValue(1, 1.5);
  encoder.setChannelValue(2, -0.75);
  encoder.setChannelValue(3, 0.75);
  encoder.setChannelValue(4, -0.5);
  encoder.setChannelValue(5, 0.5);
  encoder.setChannelValue(6, -0.25);
  encoder.setChannelValue(7, 0.25);
  encoder.setChannelValue(8, 0);

  sleep_ms(50);
  expectChannels(decoder, (const double[]){-1, 1, -0.75, 0.75, -0.5, 0.5, -0.25, 0.25, 0}, "full range");

  double expected[9];
  for (double v = -1; v < 1; v += 0.05) {
    for (int ch = 0; ch < 9; ch++) {
      encoder.setChannelValue(ch, v);
      expected[ch] = v;
    }

    sleep_ms(50);
    expectChannels(decoder, expected, "rapid changes");
  }

  printf("Test complete!");
  while(1) tight_loop_contents();

  return 0;
}