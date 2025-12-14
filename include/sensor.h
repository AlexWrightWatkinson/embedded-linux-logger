#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

struct sensor_sample
{
  uint64_t timestamp_ns;
  int32_t value;
};

void sensor_init(int32_t start_value);
void sensor_sample(struct sensor_sample *out);

#endif
