#ifndef INPUTS_H
#define INPUTS_H

#include "config.h"

#include <stddef.h>
#include <stdbool.h>


int inputs_init(const input_config_t config[], size_t len);
bool inputs_task(uint8_t* inputs);

#endif // INPUTS_H
