#ifndef EDLOGD_H
#define EDLOGD_H

#include <stdatomic.h>
#include <signal.h>
#include <stdbool.h>

typedef struct
{
  atomic_bool running;
  atomic_bool reload_requested;
} edlogd_state_t;

extern edlogd_state_t g_edlogd_state;

int setup_signal_handlers(void);

#endif
