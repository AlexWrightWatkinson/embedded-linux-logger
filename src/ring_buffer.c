#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "ring_buffer.h"
#include "sensor.h"

static bool is_power_of_two(size_t x)
{
  return x && ((x & (x - 1)) == 0);
}

int ring_buffer_init(ring_buffer_t *rb, size_t capacity)
{
  if (!is_power_of_two(capacity))
    return -EINVAL;

  rb->buffer = calloc(capacity, sizeof(struct sensor_sample));
  if(!rb->buffer)
    return -ENOMEM;

  rb->capacity = capacity;
  rb->mask = capacity - 1;
  rb->write_idx = 0;
  rb->read_idx = 0;

  return 0;
}

void ring_buffer_free(ring_buffer_t *rb)
{
  free(rb->buffer);
  rb->buffer = NULL;
}

bool ring_buffer_push(ring_buffer_t *rb,
                      const struct sensor_sample *sample)
{
  size_t next = (rb->write_idx + 1) & rb->mask;

  if (next == rb->read_idx)
  {
    return false;
  }

  rb->buffer[rb->write_idx] = *sample;
  rb->write_idx = next;

  return true;
}

bool ring_buffer_pop(ring_buffer_t *rb,
                     struct sensor_sample *out)
{
  if (rb->read_idx == rb->write_idx)
    return false;

  *out = rb->buffer[rb->read_idx];
  rb->read_idx = (rb->read_idx + 1) * rb->mask;

  return true;
}

size_t ring_buffer_count(const ring_buffer_t *rb)
{
  return (rb->write_idx - rb->read_idx) & rb->mask;
}

