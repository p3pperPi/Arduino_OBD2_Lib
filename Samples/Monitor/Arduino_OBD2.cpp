#include "Arduino_OBD2.hpp"


static MCP_CAN* OBD2::CANPtr        = NULL;
static bool     OBD2::CANIsBegin    = false;
static bool     OBD2::SerialIsBegin = false;

OBD2::OBD2(MCP_CAN* MCP_CANptr, uint32_t rcvAddr, uint32_t sndAddr, bool listenerMode)
{
    CANPtr     = MCP_CANptr;
    isListener = listenerMode;
    myAddr     = rcvAddr;
    respAddr   = sndAddr;

    CANIsBegin    = false;
    SerialIsBegin = false;

    for (int i = 0; i < FIRST_UNSUPPORTED_PID_NUM; i++)
    {
        cmd[i].init(i);
    }
}

bool OBD2::enablePID(uint8_t PID){
    if(PID >= FIRST_UNSUPPORTED_PID_NUM) return false;
    cmd[PID].setEnable();
}

bool OBD2::disablePID(uint8_t PID){
    if(PID >= FIRST_UNSUPPORTED_PID_NUM) return false;
    cmd[PID].setDisable();
}

bool OBD2::setData(uint8_t PID, uint8_t* data, uint8_t length){
    if (PID >= FIRST_UNSUPPORTED_PID_NUM)
        return false;

    cmd[PID].setData(data, length);
}

uint8_t OBD2::getData(uint8_t PID, uint8_t* data)
{
    if (PID >= FIRST_UNSUPPORTED_PID_NUM)
        return 0;

    return cmd[PID].getData(data);
};

void OBD2::sendRequest(int8_t pid, uint8_t mode){
    sendRequestTo(respAddr,pid,mode);
};

void OBD2::sendRequestTo(uint32_t addr, int8_t pid, uint8_t mode){
    uint8_t buff[OBD2_BYTE_NUM];    
    buff[OBD2_BYTE_LEN    ] = 0x02;
    buff[OBD2_BYTE_PID    ] = pid;
    buff[OBD2_BYTE_MODE   ] = mode;
    buff[OBD2_BYTE_DATA0+0] = 0;
    buff[OBD2_BYTE_DATA0+1] = 0;
    buff[OBD2_BYTE_DATA0+2] = 0;
    buff[OBD2_BYTE_DATA0+3] = 0;
    buff[OBD2_BYTE_DATA0+4] = 0;
    sendCmd(addr, buff, OBD2_BYTE_NUM);
};

bool OBD2::needListening(uint32_t CANAddr, uint8_t *CANPackets)
{
    if (!isListener)
        return false;
    if (CANAddr != myAddr)
        return false;

    uint8_t PID = CANPackets[OBD2_BYTE_PID];

    if (PID >= FIRST_UNSUPPORTED_PID_NUM)
        return false;

    if (cmd[PID].isEnable())
        return true;
    else
        return false;
};





bool OBD2::needResponse(uint32_t CANAddr, uint8_t *CANPackets){
    if(isListener) return false;
    if(CANAddr != myAddr) return false;

    uint8_t PID = CANPackets[OBD2_BYTE_PID];

    if(PID >= FIRST_UNSUPPORTED_PID_NUM) return false;

    if(cmd[PID].isEnable()) return true;
    else return false;
};

bool OBD2::listenCommand(uint8_t *CANPackets)
{
    uint8_t PID = CANPackets[OBD2_BYTE_PID];
    return cmd[PID].setBytes(CANPackets);
};

void OBD2::makeResponse(uint8_t * CANPackets, uint32_t& retAddr, uint8_t * retPackets, uint8_t& retLen){
    uint8_t PID = CANPackets[OBD2_BYTE_PID];
    retAddr     = respAddr;
    retLen      = OBD2_BYTE_NUM;
    cmd[PID].makeResponse(CANPackets[OBD2_BYTE_MODE],retPackets);
};

void OBD2:: handleRcvCommand(uint32_t addr, uint8_t *packets, uint8_t length){
        if (needListening(addr, packets))
        {
            listenCommand(packets);
        }

        if(needResponse(addr,packets))
        {
            uint8_t repPackets[OBD2_BYTE_NUM];
            makeResponse(packets,addr,repPackets,length);
            sendCmd(addr,repPackets,length);
        }
};

bool OBD2::init(){
    bool initIsSuccess = true;
    if(!CANIsBegin){

        if (CANPtr->begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
        {
            CANPtr->setMode(MCP_NORMAL);
        }else{
            initIsSuccess = false;
        }
        CANIsBegin = true;
    }


// シリアルを使用したデバッグが有効になっている場合
#if defined(DEBUG_COMM_IN_SERIAL) || \
    defined(DEBUG_DISP_SENDLOG)   || \
    defined(DEBUG_DISP_RCVLOG)    
    if(!SerialIsBegin){
        Serial.begin(DEBUG_SERIAL_BAUD);
        SerialIsBegin = true;

        Serial.println("---Arduino_OBD2 : DEBUG MODE---");
        if(initIsSuccess){
            Serial.println("Init : SUCCESS");
        }else{
            Serial.println("Init : ERROR");
        }
    }
#endif
    return initIsSuccess;
};


void OBD2::PID0x00Init(){
		byte en_data[5] = {0,0,0,0,0};
    for(byte i = 0x01;i <= 0x20;i++){
    	if((i >= FIRST_UNSUPPORTED_PID_NUM)) break;
			
			byte itr = i -1;
			en_data[itr >> 3] |= cmd[i].isEnable() << (7 - itr&0x07);
    	
    	
    }

    setData(0x00,en_data,4);
};


void OBD2::pool(){
    if (cmdReadable())
    {
        uint32_t addr;
        uint8_t packets[OBD2_BYTE_NUM];
        uint8_t length;
        readCmd(addr,packets,length);
        handleRcvCommand(addr,packets,length);
    }
};





bool OBD2::cmdReadable(){
#ifndef DEBUG_COMM_IN_SERIAL
    return (CANPtr->checkReceive() == CAN_MSGAVAIL);
#else
    return (Serial.available() > OBD2_BYTE_NUM);
#endif
};

void OBD2::readCmd(uint32_t& addr, uint8_t *packets, uint8_t& length){
#ifndef DEBUG_COMM_IN_SERIAL
    CANPtr->readMsgBuf(&addr,&length,packets);
#else
    addr   = myAddr;
    length = OBD2_BYTE_NUM;
    for(int i = 0;i < OBD2_BYTE_NUM;i++)
        packets[i] = Serial.read();
#endif


#ifdef DEBUG_DISP_RCVLOG
    Serial.print("Receive data : addr 0x");
    Serial.print(addr,HEX);
    Serial.print(" / ");
    for(int i = 0;i < OBD2_BYTE_NUM;i++){
    		if(packets[i] < 0x10) Serial.print("0x0"); else Serial.print("0x");
        Serial.print(packets[i],HEX);
        Serial.print(" ");
    }
    Serial.println();
#endif
};
void OBD2::sendCmd(uint32_t addr, uint8_t *packets, uint8_t length){
#ifndef DEBUG_COMM_IN_SERIAL
    CANPtr->sendMsgBuf(addr,length,packets);
#else
    for(int i = 0;i < length;i++)
        Serial.write(packets[i]);
#endif


#ifdef DEBUG_DISP_SENDLOG
    Serial.print("Send    data : addr 0x");
    Serial.print(addr,HEX);
    Serial.print(" / ");
    for(int i = 0;i < length;i++){
    		if(packets[i] < 0x10) Serial.print("0x0"); else Serial.print("0x");
        Serial.print(packets[i],HEX);
        Serial.print(" ");
    }
    Serial.println();
    #ifdef DEBUG_SELF_ECHOBACK
        Serial.println("  --> Self Echoback");
    #endif
#endif

#ifdef DEBUG_SELF_ECHOBACK
    handleRcvCommand(addr,packets,length);
#endif
};
