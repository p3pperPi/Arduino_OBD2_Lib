// Monitor.ino
//  特定のデバイスから送信されたデータをモニタリングする為のプログラム
//  自分からはリクエストを送信しない


#include "Arduino_OBD2.hpp"
// →下記1つをコメントアウト解除
// #define DEBUG_DISP_RCVLOG

// MCP_CANの初期化
MCP_CAN CAN(10);

        //constructor 
        // 引数
        // uint8_t  MCP_PinNo    : MCP2515のチップセレクトピンのピン番号 →ちがう
        // uint32_t rcvAddr      : 自分のアドレス＝自分が反応する必要のあるアドレス
        // uint32_t sndAddr      : 返信先のアドレス(ListenMode=trueの時は無効)
        // bool     listenerMode : 受信のみ(何が来ても返信しない、情報取得のみ)モードのON/OFF、省略するとON
OBD2 obd2_mst(&CAN,0x7DF,0x7E8,true);
OBD2 obd2_slv(&CAN,0x7E8,0x7E8,true);




void setup()
{
	// 有効にするPIDの設定
	for(int i = 0;i < 0x40;i++){
		obd2_mst.enablePID(i);
		obd2_slv.enablePID(i);
	}
	
	// CANとかの初期化
	obd2_mst.init();
	obd2_slv.init();
}


void loop() {
	// obd2の信号処理
	obd2_mst.pool();
	obd2_slv.pool();
}
