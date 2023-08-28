#include "OBD2_Command.hpp"

OBD2_Command::OBD2_Command(uint8_t PID, bool enable){
    init(PID,enable);
}

void OBD2_Command::init(uint8_t PID, bool enable){
    for(int i = 0;i < OBD2_BYTE_NUM;i++){
        cmd[i] = 0;
    }
    cmdEnable = enable;    
    cmd[OBD2_BYTE_PID] = PID;
}

void OBD2_Command::setEnable(bool enable)
{
    cmdEnable = enable;
}

void OBD2_Command::setDisable()
{
    cmdEnable = false;
}

bool OBD2_Command::isEnable(){
    return cmdEnable;
}


uint8_t OBD2_Command::getPID(){
    return cmd[OBD2_BYTE_PID];
}


bool OBD2_Command::setData(uint8_t *data, uint8_t num){
    if(num > OBD2_MAX_DATA_NUM) return false;
    vLength = num;
    for (int i = 0; i < num; i++)
    {
        value[i] = data[i];
    }
}

uint8_t OBD2_Command::getData(uint8_t *data)
{
    for (int i = 0; i < OBD2_MAX_DATA_NUM; i++)
    {
        data[i]  = value[i];
    }

    return vLength;
}

bool OBD2_Command::setBytes(uint8_t *data)
{
    vLength = cmd[OBD2_BYTE_LEN] - OBD2_BYTE_NONDATA_NUM;
    for (int i = 0; i < OBD2_BYTE_NUM; i++)
    {
        cmd[i] = data[i];
        if(i >= OBD2_BYTE_DATA0){
            value[i - OBD2_BYTE_DATA0] = cmd[i];
        }
    }
    return true;
}

uint8_t OBD2_Command::getBytes(uint8_t *data)
{

    cmd[OBD2_BYTE_LEN] = vLength + OBD2_BYTE_NONDATA_NUM;
    for (int i = 0; i < OBD2_BYTE_NUM; i++)
    {
        if (i >= OBD2_BYTE_DATA0)
        {
            cmd[i] = value[i - OBD2_BYTE_DATA0];
        }
        data[i] = cmd[i];
    }
    return vLength + OBD2_BYTE_NONDATA_NUM;
}

void OBD2_Command::makeResponse(uint8_t mode, uint8_t *retPackets)
{
    cmd[OBD2_BYTE_LEN]  = vLength + OBD2_BYTE_NONDATA_NUM;
    cmd[OBD2_BYTE_MODE] = mode | OBD2_MODE_RESP_BIT;
    for (int i = 0; i < OBD2_MAX_DATA_NUM; i++)
    {
        cmd[OBD2_BYTE_DATA0 + i] = value[i];
    }

		for(int i = 0;i < OBD2_BYTE_NUM;i++){
	    retPackets[i] = cmd[i];			
		}
}
