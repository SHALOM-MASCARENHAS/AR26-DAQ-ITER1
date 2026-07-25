#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_types.h"

/* Shared DAQ Data */
extern DAQ_Data_t g_daqData;

/* Initialization */
void SharedData_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* SHARED_DATA_H */
