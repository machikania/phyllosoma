　　　　　　　　　　　　　　　　　　　　　　　　　　2025.11.23
　Raspberry Pi Pico用BASIC実行環境オープンプラットフォーム
　　　　　　　　「MachiKania type P」
　　　　　　　　　　　　　　　　　　　　　by KenKen & Katsumi

MachiKania（マチカニア）はBASICコンパイラを搭載したマイコン用
オープンプラットフォームです。
MachiKania type PではRaspberry Pi Pico (Pico 2, Pico W)と小型
液晶モジュールを搭載し、ポータブルなBASIC実行環境を実現してい
ます。
汎用I/OやSPI、I2Cといった外部機器の制御もBASICから簡単に行う
ことが可能です。

このドキュメントでは、Waveshare RP2350-LCD-1.47（以降  
RP2350-LCD-1.47と記載）用のMachiKaniaについて述べます。

搭載しているBASICコンパイラはKM-BASICです。KM-BASICは32bit
整数型のBASICです。
また、単精度浮動小数点演算もサポートしています。
構造化プログラミング、オブジェクト指向対応も行っています。
BASICプログラムはPC等で作成し、MMCまたはSDカード経由で転送する
ことや、USBケーブルと専用ソフトを用いて転送することが可能です。
また、USBキーボードを接続し、直接プログラムを編集して実行する
ことも可能です。

同梱される実行形式ファイル（uf2ファイル）は非商用利用に限り
無償で自由に利用することが可能です。
また、利用した作品を一般に公開することも可能です。その場合、
MachiKaniaシリーズを利用していることについてもWebサイトや
説明書等、いずれかの場所に記載してください。
なお、ご利用によって生じた被害や損害については責任を負いかねます。


RP2350-LCD-1.47 対応版のMachiKaniaの使用については、以下の点に注意し
てください。


＜MACHIKAP.INI＞

MachiKaniaをRP2350-LCD-1.47で正常動作させる為に、同梱のMACHIKAP.INIを、
必ずMMC/SDカードのルートに配置してください。


＜キーボード＞

RP2350-LCD-1.47では、I/Oポートを接続しなければ、キーボードが唯一の入力
デバイスです。USB-OTGケーブルを用いて、USBキーボードを接続してください。
USB-OTGケーブルは、Type-Cで電源入力付きのものを利用して下さい。
なお、電源はRP2350-LCD-1.47の5VピンとGNDピン（10、11番ピン）からも供給可能です。

一般的な使用には、USBキーボード対応のphyllosoma_kb.uf2をインス
トールしてください。phyllosoma.uf2は、USBキーボードを使用せず、
入力なしで実行する・PCとシリアル通信で使用する、などの特殊
な用途向けです。


＜ボタン＞

MachiKaniaは、上下左右とFire/Startの、６つのボタンを使って操作しま
す。RP2350-LCD-1.47にはこれらのボタンが無いので、キーボードでエミュ
レートしています。デフォルトでは、上下左右キーと、Fireがスペース
キー、Startが改行キーに対応しています。これらの対応付けを変更す
る際は、MACHIKAP.INIを編集してください。「EMULATEBUTTONUP=」など
の設定がそれです。右は仮想キーコードを10進数で表記します。特定の
キーの仮想キーコードを知りたいときは、次のBASICプログラムを実行
して、キーを押してください。

　DO:PRINT INKEY():LOOP


＜BASICプログラムの途中停止＞

BASICプログラムを実行途中で停止したい場合、PauseキーまたはCtrl+Alt+Delキーを同時
に押してください。プログラムが途中停止します。ただし、PRINT命令な
どを一切使わないループの中などでは、停止しない場合もあります。


＜液晶ディスプレイ＞

RP2350-LCD-1.47の液晶ディスプレイはコーナーが丸くなっているため、
四隅の表示が欠けていることにご注意ください。


＜machikania-rl.zipファイルコンテンツ＞

[rp2350_lcd_1_47]ディレクトリ
　Raspberry Pi Pico 2 で使うバイナリーを格納
　-phyllosoma.uf2
　　MachiKania type P BASICシステム本体（PC connect機能対応版）
　　PCとRP2350-LCD-1.47をUSB接続し書き込む
　　PC connect機能により、USBケーブルで接続したPCからBASICプログラムを転送可能

　-phyllosoma_kb.uf2
　　MachiKania type P BASICシステム本体（USBキーボード接続対応版）
　　PCとRP2350-LCD-1.47をUSB接続し書き込む
　　内蔵エディタとUSBキーボードでBASICプログラムを直接編集、実行可能

[pcconnect]ディレクトリ
　USBケーブルを介してPC内のファイルをMachiKaniaに転送するためのツール類
　を格納

readme.txt
　このファイル

readmeLib.txt
　LIB ディレクトリー内のライブラリー説明書

＜ここより下のファイルおよびディレクトリ全てをMMC/SDカードのルートディレクトリにコピーしてください＞

MACHIKAP.INI
　MachiKania type Pの初期設定ファイル

[LIB]ディレクトリ
　BASICプログラムから簡単に利用可能なクラスライブラリ
　MMC/SDカードのルートにLIBディレクトリごとコピーして利用

[docs]ディレクトリ
　ドキュメント類を格納
　- help.txt
　- help-e.txt
　　KM-BASICのリファレンスマニュアル

　- class.txt
　- class-e.txt
　　KM-BASICでオブジェクト指向化プログラミングのリファレンスマニュアル

　- cpuclock.txt
　- cpuclock-e.txt
　　CPUのクロック周波数を調整する方法について

　- keyboard.txt
　- keyboard-e.txt
　　キーボードを接続して使用する方法について

　- pcconnect.txt
　- pcconnect-e.txt
　　PCからMachiKaniaにファイルを転送するための方法について

[samples]ディレクトリ
　BASICサンプルプログラム。これらのファイルをSDカードのルートディレクトリにコピーする
　（または任意のサブディレクトリを作成してコピー）

　3DWAVE.BAS
　　波紋の3Dグラフィック

　BLOCK.BAS
　　初代MachiKania向けに作成したブロック崩しゲーム

　COSMOS.BMP（PHOTO.BASで使用）

　FILEMAN.BAS
　　MachiKania type P/PU用ファイルマネージャ

　INVADE.BAS
　　MachiKania type Z/M/P/PU向けに作成したインベーダーゲーム

　LCHIKA.BAS
　　LEDを点滅させるサンプルプログラム

　MANDELBR.BAS
　　グラフィックでマンデルブローを出力するプログラム

　MAZE3D.BAS
　　立体迷路脱出ゲーム

　MOZART.BAS
　　WAVファイル再生サンプルプログラム

　MOZART.WAV
　　MOZART.BASで使用

　MUSIC.BAS
　　MUSIC命令使用のサンプルプログラム

　NIHONGO.BAS
　　ライブラリーのCKNJ16クラスを利用して日本語を表示するプログラム

　PCG.BAS
　　PCGを利用したサンプルプログラム

　PEGSOL-G.BAS
　　ペグソリテアゲームプログラム

　PHOTO.BAS
　　BMPファイルを液晶表示するプログラム

　RAYTRACE.BAS
　　レイトレーシングプログラム

　SOUND.BAS
　　SOUND命令使用のサンプルプログラム

　STARTREK.BAS
　　スタートレックゲームプログラム

　TIME-INT.BAS
　　タイマー割り込みのサンプルプログラム

　WFRAME.BAS
　　ワイヤーフレームグラフィックプログラム


------------------------------------------------------------------------
MachiKania type P BASICシステム改版履歴

Pyllosoma 1.61/KM-1511 (2025.12.27)
　・Waveshare RP2350-LCD-1.47に対応
