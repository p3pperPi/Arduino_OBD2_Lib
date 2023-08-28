#ifndef _OBD2_COMMAND_HPP_
#define _OBD2_COMMAND_HPP_

#include <Arduino.h>

#define OBD2_MAX_DATA_NUM 5
#define OBD2_BYTE_NUM     8
#define OBD2_BYTE_NONDATA_NUM 2

#define OBD2_BYTE_LEN   0
#define OBD2_BYTE_MODE  1
#define OBD2_BYTE_PID   2
#define OBD2_BYTE_DATA0 3

#define OBD2_MODE_RESP_BIT 0x40

// MODEは現状未実装
// 必要があれば追加？

class OBD2_Command{
public:
    OBD2_Command(uint8_t PID = 0,bool enable = false);
    void    init(uint8_t PID  ,bool enable=false);
    void    setEnable(bool enable=true);
    void    setDisable();

    bool    isEnable();
    uint8_t getPID();

    //パケット内のデータのみを入力/出力
    bool    setData(uint8_t* data, uint8_t num = 5);
    uint8_t getData(uint8_t* data); // 返り値：データ長さ

    //CANコマンド全て(8uint8_t)を全て入力/出力
    bool    setBytes(uint8_t *data);
    
    // 使わない？仮
    uint8_t getBytes(uint8_t *data); // 返り値：データ長さ

    // 返信作成
    void makeResponse(uint8_t mode,uint8_t *retPackets);

    // Todo 実装予定
    // bool    setValue(float val);
    // float   getValue();

/*
    bool isReadable();
    void readCmd(uint32_t& addr, uint8_t *packets, uint8_t& length);
    void sendCmd(uint32_t  addr, uint8_t* packets, uint8_t  length);
*/

private:
    // 値、ステータスを複数保存する可能性があるためcmdを直接編集しない
    bool cmdEnable;
    uint8_t value[OBD2_MAX_DATA_NUM];
    uint8_t vLength;
    uint8_t cmd[OBD2_BYTE_NUM];
};


//Todo : 変換関数のリスト作る



#endif
