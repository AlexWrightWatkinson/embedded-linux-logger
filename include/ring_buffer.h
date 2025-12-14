#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* SPSC
 * capacity must be pow of 2
 */

typedef struct
{
  struct sensor_sample *buffer;
  size_t capacity;
  size_t write_idx;
  size_t read_idx;
  size_t mask;
} ring_buffer_t;

int ring_buffer_init(ring_buffer_t *rb, size_t capacity);
void ring_buffer_free(ring_buffer_t *rb);

bool ring_buffer_push(ring_buffer_t *rb,
    const struct sensor_sample *sample);

bool ring_buffer_pop(ring_buffer_t *rb,
    struct sensor_sample *out);

size_t ring_buffer_count(const ring_buffer_t *rb);

#endif
