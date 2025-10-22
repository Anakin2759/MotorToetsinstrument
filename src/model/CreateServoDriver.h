#pragma once
#include "EtherCatMaster.h"
#include "ServoDriver_szhc402.h"
inline IServoDriver* CreateServoDriver(EtherCatMaster& master, const size_t& index)
{
    if (master.getSlaveInfoList()[index].product_code == 0x000000402 &&
        master.getSlaveInfoList()[index].vendor_id == 0x0000092b)
    {
        IServoDriver* servoDriver = new ServoDriver_szhc402(master, index);
        return servoDriver;
    }
    else
    {
        LOG_ERROR("不支持的从站类型");
    }
    return nullptr;
}
