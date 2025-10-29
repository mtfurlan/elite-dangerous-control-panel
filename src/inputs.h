#ifndef INPUTS_H
#define INPUTS_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "config.h"

#include <stddef.h>
#include <stdbool.h>


int inputs_init(const input_config_t config[], size_t len);
bool inputs_task(uint32_t* inputs);

#ifdef __cplusplus
 }
#endif
#endif // INPUTS_H
