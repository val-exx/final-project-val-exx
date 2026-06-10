#ifndef DIAGNOSTIC_DAEMON_H
#define DIAGNOSTIC_DAEMON_H

#include <stdint.h>

#define DIAGNOSTIC_DAEMON_VERSION "0.1.0"

typedef enum {
    MACHINE_STATE_STOPPED = 0,
    MACHINE_STATE_RUNNING = 1,
    MACHINE_STATE_FAULT = 2
} machine_state_t;

typedef struct {
    machine_state_t state;
    float temperature_c;
    float vibration_g;
    uint32_t fault_code;
    uint32_t cycle_count;
} machine_status_t;

void diagnostic_status_init(machine_status_t *status);
void diagnostic_status_update(machine_status_t *status);
void diagnostic_status_print(const machine_status_t *status);

#endif
