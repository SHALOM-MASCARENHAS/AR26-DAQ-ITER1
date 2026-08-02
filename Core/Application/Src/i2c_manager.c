#include "i2c_manager.h"

static osMutexId_t i2cMutex;

static const osMutexAttr_t i2cMutexAttributes =
{
    .name = "I2CMutex"
};

void I2C_Manager_Init(void)
{
    i2cMutex = osMutexNew(&i2cMutexAttributes);
}

void I2C_Lock(void)
{
    osMutexAcquire(i2cMutex, osWaitForever);
}

void I2C_Unlock(void)
{
    osMutexRelease(i2cMutex);
}
