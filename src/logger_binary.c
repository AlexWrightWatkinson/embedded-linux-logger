#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "logger_binary.h"

/* explicit binary record layout */

struct __attribute__((packed)) binary_log_record {
    uint64_t timestamp_ns;
    int32_t  value;
};

typedef struct {
    int fd;
} binary_logger_ctx_t;

static int binary_init(logger_t *self)
{
    /* Interface requires this parameter */
    (void)self;
    return 0;
}

static void binary_log_sample(logger_t *self,
                              const struct sensor_sample *sample)
{
    binary_logger_ctx_t *ctx = self->ctx;

    struct binary_log_record rec = {
        .timestamp_ns = sample->timestamp_ns,
        .value        = sample->value
    };

    ssize_t ret = write(ctx->fd, &rec, sizeof(rec));
    (void)ret;
}

static void binary_flush(logger_t *self)
{
    binary_logger_ctx_t *ctx = self->ctx;
    fsync(ctx->fd);
}

static void binary_close(logger_t *self)
{
    binary_logger_ctx_t *ctx = self->ctx;
    close(ctx->fd);
    free(ctx);
}

int logger_binary_create(logger_t *logger,
                         const char *path)
{
    binary_logger_ctx_t *ctx = calloc(1, sizeof(*ctx));
    if (!ctx)
        return -1;

    ctx->fd = open(path,
                   O_CREAT | O_WRONLY | O_APPEND,
                   0644);
    if (ctx->fd < 0) {
        free(ctx);
        return -1;
    }

    logger->ctx        = ctx;
    logger->init       = binary_init;
    logger->log_sample = binary_log_sample;
    logger->flush      = binary_flush;
    logger->close      = binary_close;

    return 0;
}

