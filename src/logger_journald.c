#include <stdio.h>
#include "logger.h"

static int journal_init(logger_t *self)
{
  (void)self;
  return 0;
}

static void journal_log_sample(logger_t *self,
                               const struct sensor_sample *sample)
{
  (void)self;
  fprintf(stdout,
          "sample ts=%lu value=%d\n",
          sample->timestamp_ns,
          sample->value);
}

static void journal_flush(logger_t *self)
{
  (void)self;
  fflush(stdout);
}

static void journal_close(logger_t *self)
{
  (void)self;
}

int logger_journald_create(logger_t *logger)
{
  logger->ctx = NULL;
  logger->init = journal_init;
  logger->log_sample = journal_log_sample;
  logger->flush = journal_flush;
  logger->close = journal_close;

  return 0;
}
