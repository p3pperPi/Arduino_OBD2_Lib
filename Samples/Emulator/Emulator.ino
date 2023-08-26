// Emulator.ino
//  一つのArduinoでCANの送信側/受信側の挙動を再現する為のプログラム
//  自分が送信したセンサデータのリクエストに対し自分で返答する
//  (SELF_ECHOBACK)
//  コマンドモニタの動作確認の為に使用

#include "Arduino_OBD2.hpp"
// →下記3つをコメントアウト解除
// #define DEBUG_SELF_ECHOBACK
// #define DEBUG_DISP_SENDLOG
// #define DEBUG_DISP_RCVLOG


// MCP_CANの初期化
MCP_CAN CAN(10);

// MCP_CANの初期化

        //constructor 
        // 引数
        // uint8_t  MCP_PinNo    : MCP2515のチップセレクトピンのピン番号 →ちがう
        // uint32_t rcvAddr      : 自分のアドレス＝自分が反応する必要のあるアドレス
        // uint32_t sndAddr      : 返信先のアドレス(ListenMode=trueの時は無効)
        // bool     listenerMode : 受信のみ(何が来ても返信しない、情報取得のみ)モードのON/OFF、省略するとON
OBD2 obd2(&CAN,0x7DF,0x7E8,false);

// 時間管理用の変数
unsigned long nextTime =    0;
unsigned long interval = 1000;





void setup()
{
	// 有効にするPIDの設定
	obd2.enablePID(0x00);

	// 1- 4
	obd2.enablePID(0x01);
	obd2.enablePID(0x02);
	obd2.enablePID(0x03);
	obd2.enablePID(0x04);

	// 5- 8
	obd2.enablePID(0x05);
	obd2.enablePID(0x06);
	obd2.enablePID(0x07);

	// 9- C
	obd2.enablePID(0x0C);

	// D-10
	obd2.enablePID(0x0F);

  //11-14
	obd2.enablePID(0x12);

  //15-18
	obd2.enablePID(0x15);

  //19-1C
  obd2.enablePID(0x1A);

	//1D-20
  obd2.enablePID(0x1F);
  obd2.enablePID(0x20);



	// CANとかの初期化
	obd2.init();


	// PIPD0x00のパラメータセット
	obd2.PID0x00Init();


	//有効なPIDの確認
	obd2.sendRequestTo(0x7DF,0x00);

	//定期実行処理の設定
	nextTime  = millis() + interval;
}


uint8_t tergetPID = 0x01;
uint8_t data[5];

void loop() {

	if(nextTime < millis()){
	// interval[ms] ごとに1回だけ実行する処理

		// 適当にデータを設定	
		data[0] ++;
		data[1] = data[0] + 1;
		data[2] = data[1] + 1;
		data[3] = data[2] + 1;
		data[4] = data[3] + 1;

		// 設定したデータを反映
		obd2.setData(tergetPID,data,5);

		// データをリクエスト
		obd2.sendRequestTo(0x7DF,0x01);

		//時間の更新
		nextTime += interval;
	}

	// obd2の信号処理
	obd2.pool();
}
