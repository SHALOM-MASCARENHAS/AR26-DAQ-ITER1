#ifndef I2C_MANAGER_H
#define I2C_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"

void I2C_Manager_Init(void);

void I2C_Lock(void);
void I2C_Unlock(void);

#ifdef __cplusplus
}
#endif

#endif
