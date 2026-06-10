#include "diagnostic_daemon.h"

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static volatile sig_atomic_t keep_running = 1;

static void handle_signal(int signal_number)
{
    (void)signal_number;
    keep_running = 0;
}

void diagnostic_status_init(machine_status_t *status)
{
    if (status == NULL) {
        return;
    }

    status->state = MACHINE_STATE_RUNNING;
    status->temperature_c = 35.0f;
    status->vibration_g = 0.10f;
    status->fault_code = 0;
    status->cycle_count = 0;
}

void diagnostic_status_update(machine_status_t *status)
{
    if (status == NULL) {
        return;
    }

    status->cycle_count++;
    status->temperature_c += 0.2f;
    status->vibration_g += 0.01f;

    if (status->temperature_c >= 45.0f) {
        status->state = MACHINE_STATE_FAULT;
        status->fault_code = 1001;
    }
}

void diagnostic_status_print(const machine_status_t *status)
{
    if (status == NULL) {
        return;
    }

    const char *state_string = "UNKNOWN";

    switch (status->state) {
    case MACHINE_STATE_STOPPED:
        state_string = "STOPPED";
        break;
    case MACHINE_STATE_RUNNING:
        state_string = "RUNNING";
        break;
    case MACHINE_STATE_FAULT:
        state_string = "FAULT";
        break;
    default:
        break;
    }

    printf("[diagnostic-daemon] version=%s state=%s temperature=%.2fC vibration=%.2fg fault_code=%u cycle=%u\n",
           DIAGNOSTIC_DAEMON_VERSION,
           state_string,
           status->temperature_c,
           status->vibration_g,
           status->fault_code,
           status->cycle_count);

    fflush(stdout);
}

int main(void)
{
    machine_status_t status;

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    diagnostic_status_init(&status);

    printf("[diagnostic-daemon] starting\n");
    fflush(stdout);

    while (keep_running) {
        diagnostic_status_update(&status);
        diagnostic_status_print(&status);
        sleep(2);
    }

    printf("[diagnostic-daemon] stopping\n");
    fflush(stdout);

    return EXIT_SUCCESS;
}
