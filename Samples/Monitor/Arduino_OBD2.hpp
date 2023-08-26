#ifndef _ARDUINO_OBD_HPP_
#define _ARDUINO_OBD_HPP_

#include "OBD2_Command.hpp"
#include "PID_LIST.h"

#include "Arduino.h"

// コミュニティパッケージを使用；mcp_can
#include <mcp_can.h>
#include <mcp_can_dfs.h>


#define DEBUG_SERIAL_BAUD 115200
// debug function
// →コメントアウト解除で機能有効化
// デバッグ用の機能：通常の用途では使用しない想定
//
//#define DEBUG_SELF_ECHOBACK
//  →自分が発信したコマンドを自分の受信キューに入れる
//  　1CPUで送信側：受信側のコマンドをエミュレートする際に使用
//
//#define DEBUG_COMM_IN_SERIAL
//  →CANではなくUARTでコマンド送受信
//    (デバッグ用なので、アドレスの認識はスキップする)
//
//#define DEBUG_DISP_SENDLOG
//  →コマンドの送信ログをUARTで表示(Serial.write)
//
#define DEBUG_DISP_RCVLOG
//  →コマンドの受信ログをUARTで表示
//

//
//  listenerModeについて
// true  : CAN上に乗っかっている情報を読み取るだけでk自分は何もコマンドを発しない
//         (sendRequestを実行したらリクエストを送信する)
// false : 受信アドレス(rcvAddr)に向けて送られたコマンドのみ送信アドレス(sndAddr)に返答する

class OBD2{
    public:
        /* constructor 
            @説明
                コンストラクタ
            @引数            
                MCP_CAN* MCP_CANptr   : 定義済みのMCP_CANクラスのポインタ
                uint32_t rcvAddr      : 自分のアドレス＝自分が反応する必要のあるアドレス
                uint32_t sndAddr      : 返信先のアドレス(ListenMode=trueの時は無効)
                bool     listenerMode : 受信のみ(何が来ても返信しない、情報取得のみ)モードのON/OFF、省略するとON
            */
        OBD2(MCP_CAN* MCP_CANptr, uint32_t rcvAddr, uint32_t sndAddr, bool listenerMode = true);

        /* bool    enablePID(uint8_t PID);
            @説明
                引数のPIDを有効化する。有効化されたPIDは下記の動作を行う
                　ListenerMode on  :そのPIDあての信号を読み取る
                　ListenerMode off :そのPIDのに対するリクエストが来た際に返答を行う
            @引数
                uint8_t PID : 対象のPID
            @返り値
                指定PIDの有効化状態
        */
        bool    enablePID(uint8_t PID);
        /* bool    disablePID(uint8_t PID);
            @説明
                引数のPIDを無効化する。
            @引数
                uint8_t PID : 対象のPID
            @返り値
                指定PIDの有効化状態
        */
        bool    disablePID(uint8_t PID);


        /*bool    setData(uint8_t PID, uint8_t* data, uint8_t length);
            @説明
                指定されたPIDにデータを保存する。
                対象のPIDにリクエストがあった際は保存済みのデータを返答する。
            @引数
                uint8_t  PID    : 対象のPID
                uint8_t* data   : 保存するデータ(要素数1～5の配列)
                uint8_t  length : 保存データの要素数
            @返り値
                データ保存の成否(有効化されていないPIDを指定した際はfalseが返される)
        */
        bool    setData(uint8_t PID, uint8_t* data, uint8_t length);

        /*uint8_t getData(uint8_t PID, uint8_t* data);
            @説明
                指定されたPIDに保存されたデータを取り出す。
            @引数
                uint8_t  PID    : 対象のPID
                uint8_t* data   : 保存されたデータ(要素数5の配列)
            @返り値
                データ取得の成否(有効化されていないPIDを指定した際はfalseが返される)
        */
        uint8_t getData(uint8_t PID, uint8_t* data);


        /*void    sendRequest  (              int8_t pid, uint8_t mode = 0x00);
            @説明
                指定されたPIDのデータ取得リクエストを送る。
            @引数
                uint8_t PID  : 対象のPID
                uint8_t mode : モードByteの値(省略すると0)
            @返り値
                なし
        */
        void    sendRequest  (              int8_t pid, uint8_t mode = 0x01);

        /*void    sendRequestTo(uint32_t addr,int8_t pid, uint8_t mode = 0x00);
            @説明
                指定されたアドレスのデバイスに指定されたPIDのデータ取得リクエストを送る。
            @引数
                uint32_t addr : 対象のアドレス
                uint8_t  PID  : 対象のPID
                uint8_t  mode : モードByteの値(省略すると0)
            @返り値
                なし
        */
        void    sendRequestTo(uint32_t addr,int8_t pid, uint8_t mode = 0x01);


        bool init();
        void pool();
				void PID0x00Init();

        static MCP_CAN* CANPtr;
        static bool CANIsBegin;
        static bool SerialIsBegin;


    private:
        bool         isListener;
        uint32_t     myAddr;
        uint32_t     respAddr;
        OBD2_Command cmd[FIRST_UNSUPPORTED_PID_NUM];

        bool needListening(uint32_t CANAddr, uint8_t *CANPackets);
        bool needResponse (uint32_t CANAddr, uint8_t* CANPackets);

        bool listenCommand(uint8_t *CANPackets);
        void makeResponse (uint8_t *CANPackets, uint32_t& retAddr, uint8_t *retPackets, uint8_t& retLen);

        void handleRcvCommand(uint32_t addr, uint8_t *packets, uint8_t length);

        bool cmdReadable();
        void readCmd(uint32_t& addr, uint8_t *packets, uint8_t& length);
        void sendCmd(uint32_t  addr, uint8_t *packets, uint8_t  length);
};

#endif
