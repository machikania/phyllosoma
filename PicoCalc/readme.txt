　　　　　　　　　　　　　　　　　　　　　　　　　　2025.12.27
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

このドキュメントでは、ClockworkPi PicoCalc（以降 PicoCalcと
記載）用のMachiKaniaについて述べます。

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


PicoCalc 対応版のMachiKaniaの使用については、以下の点に注意し
てください。

＜対応する Pi Pico ボード＞

Raspberry Pi Pico, Raspberry Pi Pico 2, Raspberry Pi Pico W, 
Raspberry Pi Pico 2 W の４つに対応しています。配布アーカイブの
中から、それぞれのボードに対応したphyllosoma_kb.uf2ファイルを
インストールしてください。WiFi対応版では、WiFi接続を通じて、
インターネットに接続することも可能です。

uf2ファイルのインストールは、次の手順で行います。
　１．PicoCalcの電源を切る
　２．PicoCalcを裏返し、Raspberry Pi Picoボードの「BOOTSEL」
　　　ボタンを押しながら、USB micro Bポートを通じて、PCと接続する
　３．RPI-RP2もしくはRP2350ドライブが現れるので、phyllosoma_kb.uf2
　　　ファイルをドライブにドラッグ＆ドロップする
　４．USB micro Bプラグを抜き、PicoCalcの電源を入れる

＜MACHIKAP.INI＞

MachiKaniaをPicoCalcで正常動作させる為に、同梱のMACHIKAP.INIを、
必ずMMC/SDカードのルートに配置してください。


＜キーボード＞

キーボードは、PicoCalcに内蔵のキーボードに対応しています。USB
キーボードを接続しても認識しないので、注意してください。

USBキーボードとPicoCalcキーボードでの動作の違いは、以下の通りで
す。

・「Shift + Right」「Shift + Left」が使えないので、代わりに
　「Alt + Right」「Alt + Left」を使う。
・「Page Up」「Page Down」ボタンが無いので、代わりに「Alt + Up」
　「Alt + Down」を使う。
・「Shift」キーなしで「Home」「End」を使いたい場合は、「Alt + Tab」
　「Alt + Del」を押す。


＜ボタン＞

MachiKaniaは、上下左右とFire/Startの、６つのボタンを使って操作
します。PicoCalcにはこれらのボタンが無いので、キーボードでエミュ
レートしています。デフォルトでは、上下左右キーと、Fireがスペース
キー、Startが改行キーに対応しています。これらの対応付けを変更す
る際は、MACHIKAP.INIを編集してください。「EMULATEBUTTONUP=」など
の設定がそれです。右は仮想キーコードを10進数で表記します。特定の
キーの仮想キーコードを知りたいときは、次のBASICプログラムを実行
して、キーを押してください。

　DO:PRINT INKEY():LOOP


＜I2Cの利用＞

キーボードの認識に、I2C機能を用いています（GP6, GP7; I2C1）。もし、
別の用途でI2Cを使いたい場合は、I2C1ではなく、I2C0を用いてください
（GP0/GP1など）。設定は、MACHIKAP.INIの「I2CSDA=」と「I2CSCL=」で
行います。この設定にI2C1を用いるポート(GP2/GP3など)を使うと、キー
ボードを認識しなくなるので、注意が必要です。


＜BASICプログラムの途中停止＞

BASICプログラムを実行途中で停止したい場合、Ctrl+Alt+Delキーを同時
に押してください。プログラムが途中停止します。ただし、PRINT命令な
どを一切使わないループの中などでは、停止しない場合もあります。


＜machikania-pc.zipファイルコンテンツ＞

[pico]ディレクトリ
　Raspberry Pi Pico で使うバイナリーを格納
　-phyllosoma_kb.uf2
　　MachiKania type P BASICシステム本体（キーボード接続対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　内蔵エディタとキーボードでBASICプログラムを直接編集、実行可能

[pico_w]ディレクトリ
　Raspberry Pi Pico W で使うバイナリーを格納
　-phyllosoma_kb.uf2
　　MachiKania type P BASICシステム本体（キーボード接続対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　内蔵エディタとキーボードでBASICプログラムを直接編集、実行可能
　　WiFi接続が可能

[pico2]ディレクトリ
　Raspberry Pi Pico 2 で使うバイナリーを格納
　-phyllosoma_kb.uf2
　　MachiKania type P BASICシステム本体（キーボード接続対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　内蔵エディタとキーボードでBASICプログラムを直接編集、実行可能

[pico2_w]ディレクトリ
　Raspberry Pi Pico 2 W で使うバイナリーを格納
　-phyllosoma_kb.uf2
　　MachiKania type P BASICシステム本体（キーボード接続対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　内蔵エディタとキーボードでBASICプログラムを直接編集、実行可能
　　WiFi接続が可能

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
　　キーボードの使用方法について

　- wifi.txt
　- wifi-e.txt
　　Raspberry Pi Pico Wを使ってWiFi接続を行うときのリファレンスマニュアル 

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

　HDAEMON.BAS
　　HTTPDクラスを使って web サーバー構築を構築するサンプルプログラム
　　PCやスマートフォンから接続して、Pico W の LED をオン・オフできる

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

　WEATHER.BAS
　　WGETクラスとJSONクラスを使った、ネット上のwebページから情報を得るサンプルプログラム
　　今日の日付と天気予報を表示する
　
　WFRAME.BAS
　　ワイヤーフレームグラフィックプログラム


------------------------------------------------------------------------
MachiKania type P BASICシステム改版履歴

Phyllosoma 1.60/KM-1510 (2025.8.17)
　・ClockworkPi PicoCalcに対応

Pyllosoma 1.61/KM-1511 (2025.12.27)
　・エディター使用時に、ステートメントもしくは関数のヘルプを表示できるようにした
　・RND#()関数を追加
　・ネット接続におけるTLSハンドシェイクを改善。16 Kbを超えるファイルをhttpsプロトコルで取得可能に
　・PWM4～PWM9の使用をサポートし、使用ポートをINIファイルで指定できるようにした
　・PicoCalc、ResTouch、type PU miniで、OUT8L/IN8L、OUT8H/IN8H、OUT16/IN16ステートメントの挙動を修正
　・クラスファイル中で別のクラスを使っている際、特定の環境でコンパイルできなくなる不具合を修正
　・クラスライブラリーに高速フーリエ変換および逆変換を行うためのクラス、FFTLIBを追加
　・クラスライブラリーのWGETを更新。301 Movedなどに対応
　・class.txtを修正
　・help-k.txtを追加
