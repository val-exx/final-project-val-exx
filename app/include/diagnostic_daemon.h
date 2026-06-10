#ifndef DIAGNOSTIC_DAEMON_H
#define DIAGNOSTIC_DAEMON_H

#include <stdint.h>

#define DIAGNOSTIC_DAEMON_VERSION "0.2.0"
#define DIAGNOSTIC_EVENT_LOG_PATH "/tmp/diagnostic_events.log"

typedef enum {
    MACHINE_STATE_STOPPED = 0,
    MACHINE_STATE_STARTING = 1,
    MACHINE_STATE_RUNNING = 2,
    MACHINE_STATE_WARNING = 3,
    MACHINE_STATE_FAULT = 4
} machine_state_t;

typedef enum {
    FAULT_NONE = 0,
    FAULT_OVER_TEMPERATURE = 1001,
    FAULT_HIGH_VIBRATION = 1002,
    FAULT_SENSOR_FAILURE = 1003
} fault_code_t;

typedef struct {
    machine_state_t state;
    float temperature_c;
    float vibration_g;
    fault_code_t fault_code;
    uint32_t cycle_count;
} machine_status_t;

const char *diagnostic_state_to_string(machine_state_t state);
const char *diagnostic_fault_to_string(fault_code_t fault_code);

void diagnostic_status_init(machine_status_t *status);
void diagnostic_status_update(machine_status_t *status);
void diagnostic_status_print(const machine_status_t *status);

int diagnostic_event_log_init(const char *log_path);
int diagnostic_event_log_write(const char *log_path, const machine_status_t *status, const char *event);

#endif
