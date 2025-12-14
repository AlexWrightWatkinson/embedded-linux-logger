#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include "sensor.h"

/* abstracted logging backend */

typedef struct logger
{
  int (*init)(struct logger *self);
  void (*log_sample)(struct logger *self,
      const struct sensor_sample *sample);
  void (*flush)(struct logger *self);
  void (*close)(struct logger *self);

  void *ctx;
} logger_t;

#endif
