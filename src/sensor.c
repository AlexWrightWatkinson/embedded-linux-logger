#include <time.h>
#include "sensor.h"

static int32_t current_value;

static uint64_t monotonic_time_ns(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

void sensor_init(int32_t start_value)
{
  current_value = start_value;
}

void sensor_sample(struct sensor_sample *out)
{
  out->timestamp_ns = monotonic_time_ns();
  out->value = current_value++;
}
