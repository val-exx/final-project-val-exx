#include "diagnostic_daemon.h"

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define TEMPERATURE_WARNING_THRESHOLD_C 42.0f
#define TEMPERATURE_FAULT_THRESHOLD_C 45.0f
#define VIBRATION_WARNING_THRESHOLD_G 0.35f
#define VIBRATION_FAULT_THRESHOLD_G 0.50f

static volatile sig_atomic_t keep_running = 1;

static void handle_signal(int signal_number)
{
    (void)signal_number;
    keep_running = 0;
}

const char *diagnostic_state_to_string(machine_state_t state)
{
    switch (state) {
    case MACHINE_STATE_STOPPED:
        return "STOPPED";
    case MACHINE_STATE_STARTING:
        return "STARTING";
    case MACHINE_STATE_RUNNING:
        return "RUNNING";
    case MACHINE_STATE_WARNING:
        return "WARNING";
    case MACHINE_STATE_FAULT:
        return "FAULT";
    default:
        return "UNKNOWN";
    }
}

const char *diagnostic_fault_to_string(fault_code_t fault_code)
{
    switch (fault_code) {
    case FAULT_NONE:
        return "NONE";
    case FAULT_OVER_TEMPERATURE:
        return "OVER_TEMPERATURE";
    case FAULT_HIGH_VIBRATION:
        return "HIGH_VIBRATION";
    case FAULT_SENSOR_FAILURE:
        return "SENSOR_FAILURE";
    default:
        return "UNKNOWN";
    }
}

void diagnostic_status_init(machine_status_t *status)
{
    if (status == NULL) {
        return;
    }

    status->state = MACHINE_STATE_STARTING;
    status->temperature_c = 35.0f;
    status->vibration_g = 0.10f;
    status->fault_code = FAULT_NONE;
    status->cycle_count = 0;
}

void diagnostic_status_update(machine_status_t *status)
{
    if (status == NULL) {
        return;
    }
    
    if (status->state == MACHINE_STATE_FAULT) {
    status->cycle_count++;
    return;
    }

    status->cycle_count++;

    if (status->cycle_count == 1) {
        status->state = MACHINE_STATE_STARTING;
    } else {
        status->state = MACHINE_STATE_RUNNING;
    }

    status->temperature_c += 0.35f;
    status->vibration_g += 0.02f;
    status->fault_code = FAULT_NONE;

    if (status->cycle_count == 12) {
        status->fault_code = FAULT_SENSOR_FAILURE;
        status->state = MACHINE_STATE_FAULT;
        return;
    }

    if (status->temperature_c >= TEMPERATURE_FAULT_THRESHOLD_C) {
        status->fault_code = FAULT_OVER_TEMPERATURE;
        status->state = MACHINE_STATE_FAULT;
        return;
    }

    if (status->vibration_g >= VIBRATION_FAULT_THRESHOLD_G) {
        status->fault_code = FAULT_HIGH_VIBRATION;
        status->state = MACHINE_STATE_FAULT;
        return;
    }

    if ((status->temperature_c >= TEMPERATURE_WARNING_THRESHOLD_C) ||
        (status->vibration_g >= VIBRATION_WARNING_THRESHOLD_G)) {
        status->state = MACHINE_STATE_WARNING;
    }
}

void diagnostic_status_print(const machine_status_t *status)
{
    if (status == NULL) {
        return;
    }

    printf("[diagnostic-daemon] version=%s state=%s temperature=%.2fC vibration=%.2fg fault=%s fault_code=%u cycle=%u\n",
           DIAGNOSTIC_DAEMON_VERSION,
           diagnostic_state_to_string(status->state),
           status->temperature_c,
           status->vibration_g,
           diagnostic_fault_to_string(status->fault_code),
           status->fault_code,
           status->cycle_count);

    fflush(stdout);
}

int diagnostic_event_log_init(const char *log_path)
{
    FILE *log_file = fopen(log_path, "w");

    if (log_file == NULL) {
        return -1;
    }

    fprintf(log_file, "Diagnostic daemon event log started\n");
    fclose(log_file);

    return 0;
}

int diagnostic_event_log_write(const char *log_path, const machine_status_t *status, const char *event)
{
    FILE *log_file = NULL;
    time_t now;
    struct tm *local_time = NULL;
    char timestamp[32];

    if ((log_path == NULL) || (status == NULL) || (event == NULL)) {
        return -1;
    }

    now = time(NULL);
    local_time = localtime(&now);

    if (local_time == NULL) {
        return -1;
    }

    if (strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time) == 0) {
        return -1;
    }

    log_file = fopen(log_path, "a");

    if (log_file == NULL) {
        return -1;
    }

    fprintf(log_file,
            "%s event=%s state=%s temperature=%.2fC vibration=%.2fg fault=%s fault_code=%u cycle=%u\n",
            timestamp,
            event,
            diagnostic_state_to_string(status->state),
            status->temperature_c,
            status->vibration_g,
            diagnostic_fault_to_string(status->fault_code),
            status->fault_code,
            status->cycle_count);

    fclose(log_file);

    return 0;
}

int main(void)
{
    machine_status_t status;

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    diagnostic_status_init(&status);

    printf("[diagnostic-daemon] starting\n");
    fflush(stdout);

    if (diagnostic_event_log_init(DIAGNOSTIC_EVENT_LOG_PATH) != 0) {
        fprintf(stderr, "[diagnostic-daemon] failed to initialize event log: %s\n",
                DIAGNOSTIC_EVENT_LOG_PATH);
        return EXIT_FAILURE;
    }

    diagnostic_event_log_write(DIAGNOSTIC_EVENT_LOG_PATH, &status, "DAEMON_START");

    while (keep_running) {
        diagnostic_status_update(&status);
        diagnostic_status_print(&status);

        if (status.state == MACHINE_STATE_WARNING) {
            diagnostic_event_log_write(DIAGNOSTIC_EVENT_LOG_PATH, &status, "MACHINE_WARNING");
        }

        if (status.state == MACHINE_STATE_FAULT) {
            diagnostic_event_log_write(DIAGNOSTIC_EVENT_LOG_PATH, &status, "MACHINE_FAULT");
        }

        sleep(2);
    }

    diagnostic_event_log_write(DIAGNOSTIC_EVENT_LOG_PATH, &status, "DAEMON_STOP");

    printf("[diagnostic-daemon] stopping\n");
    fflush(stdout);

    return EXIT_SUCCESS;
}
