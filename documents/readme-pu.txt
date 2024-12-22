　　　　　　　　　　　　　　　　　　　　　　　　　　2024.12.28
　Raspberry Pi Pico用BASIC実行環境オープンプラットフォーム
　　　　　　　　「MachiKania type PU」
　　　　　　　　　　　　　　　　　　　　　by KenKen & Katsumi

MachiKania（マチカニア）はBASICコンパイラを搭載したマイコン用
オープンプラットフォームです。
MachiKania type PUではRaspberry Pi Pico (Pico 2, Pico W)とNTSC
ビデオ出力回路を搭載し、ポータブルなBASIC実行環境を実現してい
ます。
汎用I/OやSPI、I2Cといった外部機器の制御もBASICから簡単に行う
ことが可能です。

搭載しているBASICコンパイラはKM-BASICです。KM-BASICは32bit
整数型のBASICです。
また、単精度浮動小数点演算もサポートしています。
構造化プログラミング、オブジェクト指向対応も行っています。
BASICプログラムはPC等で作成し、MMCまたはSDカード経由で転送する
ことや、USBケーブルと専用ソフトを用いて転送することが可能です。
また、USBキーボードを接続し、直接プログラムを編集して実行する
ことも可能です。

動作回路やその他の詳細は、下記のWebサイトを参照してください。
http://www.ze.em-net.ne.jp/~kenken/machikania/typepu.html


同梱される実行形式ファイル（uf2ファイル）は非商用利用に限り
無償で自由に利用することが可能です。
また、利用した作品を一般に公開することも可能です。その場合、
MachiKaniaシリーズを利用していることについてもWebサイトや
説明書等、いずれかの場所に記載してください。
なお、ご利用によって生じた被害や損害については責任を負いかねます。


＜machikania-pu.zipファイルコンテンツ＞

[documents]ディレクトリ
　ドキュメント類を格納
　- help.txt
　- help-e.txt
　　KM-BASICのリファレンスマニュアル

　- class.txt
　- class-e.txt
　　KM-BASICでオブジェクト指向化プログラミングのリファレンスマニュアル

　- embed.txt
　- embed-e.txt
　　ファイル埋め込み実行ファイル作成方法について

　- pcconnect.txt
　- pcconnect-e.txt
　　PCからMachiKaniaにファイルを転送するための方法について

　- shematic_ntsc.png
　- shematic_xiao.png
　　MachiKania type PU回路図

　- wifi.txt
　- wifi-e.txt
　　Raspberry Pi Pico Wを使ってWiFi接続を行うときのリファレンスマニュアル

[pico_ntsc]ディレクトリ
　Raspberry Pi Pico を使う時の使うバイナリーを格納
　-puerulus.uf2
　　MachiKania type PU BASICシステム本体（PC connect機能対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　PC connect機能により、USBケーブルで接続したPCからBASICプログラムを転送可能

　-puerulus_kb.uf2
　　MachiKania type PU BASICシステム本体（USBキーボード接続対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　内蔵エディタとUSBキーボードでBASICプログラムを直接編集、実行可能

[pico_w_ntsc]ディレクトリ
　Raspberry Pi Pico W を使う時の使うバイナリーを格納
　-puerulus.uf2
　　MachiKania type PU BASICシステム本体（PC connect機能対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　PC connect機能により、USBケーブルで接続したPCからBASICプログラムを転送可能
　　WiFi接続が可能

　-puerulus_kb.uf2
　　MachiKania type PU BASICシステム本体（USBキーボード接続対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　内蔵エディタとUSBキーボードでBASICプログラムを直接編集、実行可能
　　WiFi接続が可能

[pico2_ntsc]ディレクトリ
　Raspberry Pi Pico 2 を使う時の使うバイナリーを格納
　-puerulus.uf2
　　MachiKania type PU BASICシステム本体（PC connect機能対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　PC connect機能により、USBケーブルで接続したPCからBASICプログラムを転送可能

　-puerulus_kb.uf2
　　MachiKania type PU BASICシステム本体（USBキーボード接続対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　内蔵エディタとUSBキーボードでBASICプログラムを直接編集、実行可能

[xiao_ntsc]ディレクトリ
　XIAO-RP2040, TINY2040, RP2040-Zero を使う時の使うバイナリーを格納
　-puerulus.uf2
　　MachiKania type PU BASICシステム本体（PC connect機能対応版）
　　PCとマイコンボードをUSB接続し書き込む
　　PC connect機能により、USBケーブルで接続したPCからBASICプログラムを転送可能

　-puerulus_kb.uf2
　　MachiKania type PU BASICシステム本体（USBキーボード接続対応版）
　　PCとマイコンボードをUSB接続し書き込む
　　内蔵エディタとUSBキーボードでBASICプログラムを直接編集、実行可能

[xiao_rp2350_ntsc]ディレクトリ
　XIAO-RP2350, TINY2350, RP2350-Zero を使う時の使うバイナリーを格納
　-puerulus.uf2
　　MachiKania type PU BASICシステム本体（PC connect機能対応版）
　　PCとマイコンボードをUSB接続し書き込む
　　PC connect機能により、USBケーブルで接続したPCからBASICプログラムを転送可能

　-puerulus_kb.uf2
　　MachiKania type PU BASICシステム本体（USBキーボード接続対応版）
　　PCとマイコンボードをUSB接続し書き込む
　　内蔵エディタとUSBキーボードでBASICプログラムを直接編集、実行可能

[pico_ntsc/embed]ディレクトリ
[pico_w_ntsc/embed]ディレクトリ
[pico2_ntsc/embed]ディレクトリ
[xiao_ntsc/embed]ディレクトリ
[xiao_rp2350_ntsc/embed]ディレクトリ
　組み込み用途などMMC/SDカードなしでもBASICプログラムを実行することが
　できるようなツール類を格納

[pcconnect]ディレクトリ
　USBケーブルを介してPC内のファイルをMachiKaniaに転送するためのツール類
　を格納

readme-pu.txt
　このファイル

readmeLib.txt
　LIB ディレクトリー内のライブラリー説明書

＜ここより下のファイルおよびディレクトリ全てをSDカードのルートディレクトリにコピーしてください＞

MACHIKAP.INI
　MachiKania type PUの初期設定ファイル(type Pと共通)

[LIB]ディレクトリ
　BASICプログラムから簡単に利用可能なクラスライブラリ
　SDカードのルートにLIBディレクトリごとコピーして利用

[samples]ディレクトリ
　BASICサンプルプログラム。これらのファイルをSDカードのルートディレクトリにコピーする
　（または任意のサブディレクトリを作成してコピー）

　3DWAVE.BAS
　　波紋の3Dグラフィック

　BLOCK.BAS
　　初代MachiKania向けに作成したブロック崩しゲーム

　FILEMAN.BAS
　　MachiKania type P用ファイルマネージャ

　INVADE.BAS
　　MachiKania type Z向けに作成したインベーダーゲーム

　LCHIKA.BAS
　　LEDを点滅させるサンプルプログラム

　MANDELBR.BAS
　　テキストでマンデルブローを出力するプログラム

　MAZE3D.BAS
　　立体迷路脱出ゲーム

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

　STARTREK.BAS
　　スタートレックゲームプログラム

　SOUND.BAS
　　SOUND命令使用のサンプルプログラム

　TIME-INT.BAS
　　タイマー割り込みのサンプルプログラム

　WFRAME.BAS
　　ワイヤーフレームグラフィックプログラム

　WEATHER.BAS
　　WGETクラスとJSONクラスを使った、ネット上のwebページから情報を得るサンプルプログラム
　　今日の日付と天気予報を表示する
　
　HDAEMON.BAS
　　HTTPDクラスを使って web サーバー構築を構築するサンプルプログラム
　　PCやスマートフォンから接続して、Pico W の LED をオン・オフできる

　COSMOS.BMP（PHOTO.BASで使用）


------------------------------------------------------------------------
MachiKania type PU BASICシステム改版履歴

Phyllosoma 1.50/KM-1507（2024.12.28）
　・最初の公開バージョン
