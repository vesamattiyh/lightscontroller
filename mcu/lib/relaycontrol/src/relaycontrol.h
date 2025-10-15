#pragma once

/* Standard C Libraries */

// ESP-Related APIs

#define MAX_RELAYS 6

void relay_setup(void);
uint16_t relay_get_status(void);
bool relay_get_state(int relay_num);
bool relay_set_state(int relay_num, bool state);