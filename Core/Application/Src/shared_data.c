#include "shared_data.h"
#include <string.h>

DAQ_Data_t g_daqData;

void SharedData_Init(void)
{
    memset(&g_daqData, 0, sizeof(g_daqData));
}
