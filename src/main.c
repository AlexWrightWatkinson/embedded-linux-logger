#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "edlogd.h"
#include "sensor.h"
#include "ring_buffer.h"
#include "logger.h"
#include "logger_binary.h"

/* global daemon state */

edlogd_state_t g_edlogd_state =
{
  .running = ATOMIC_VAR_INIT(true),
  .reload_requested = ATOMIC_VAR_INIT(false),
};

/* signal handler */

static void signal_handler(int signo)
{
  switch (signo)
  {
    case SIGTERM:
    case SIGINT:
      atomic_store(&g_edlogd_state.running, false);
      break;
    case SIGHUP:
      atomic_store(&g_edlogd_state.reload_requested, true);
      break;
    default:
      break;
  }
}

int setup_signal_handler(void)
{
  struct sigaction sa;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = signal_handler;

  /* block all signals during handler exec */
  sigfillset(&sa.sa_mask);

  if (sigaction(SIGTERM, &sa, NULL) < 0)
    return -1;
  if (sigaction(SIGINT, &sa, NULL) < 0)
    return -1;
  if (sigaction(SIGHUP, &sa, NULL) < 0)
    return -1;

  return 0;
}

/* main daemon entry point */

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  fprintf(stdout, "edlogd: starting up\n");

  if (setup_signal_handler() < 0)
  {
    fprintf(stderr, "edlogd: failed to set signal handlers: %s\n",
             strerror(errno));
    return EXIT_FAILURE;
  }

  ring_buffer_t rb;
  if (ring_buffer_init(&rb, 1024) < 0)
  {
    fprintf(stderr, "edlogd: ring buffer init failed\n");
    return EXIT_FAILURE;
  }

  sensor_init(0);

  /* logger initialization */

  logger_t logger;
  if (logger_binary_create(&logger, "edlogd.bin") < 0)
  {
    fprintf(stderr, "edlogd: failed to init binary logger\n");
    return EXIT_FAILURE;
  }

  if (logger.init(&logger) < 0)
  {
    fprintf(stderr, "edlogd: logger init failed\n");
    return EXIT_FAILURE;
  }

  /* main loop */

  while (atomic_load(&g_edlogd_state.running))
  {
    if (atomic_load(&g_edlogd_state.reload_requested))
    {
      fprintf(stdout, "edlogd: reload requested\n");
      atomic_store(&g_edlogd_state.reload_requested, false);
    }

    struct sensor_sample sample;
    sensor_sample(&sample);

    if (!ring_buffer_push(&rb, &sample))
    {
      fprintf(stderr, "edlogd: ring buffer overflow\n");
    }

    else
    {
      logger.log_sample(&logger, &sample);
    }

    struct timespec ts =
    {
      .tv_sec = 0,
      .tv_nsec = 100 * 1000 * 1000
    };
    nanosleep(&ts, NULL);
  }

  fprintf(stdout, "edlogd: shutting down cleanly\n");

  logger.flush(&logger);
  logger.close(&logger);

  ring_buffer_free(&rb);

  return EXIT_SUCCESS;
}
