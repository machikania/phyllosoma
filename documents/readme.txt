　　　　　　　　　　　　　　　　　　　　　　　　　　2024.12.28
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

搭載しているBASICコンパイラはKM-BASICです。KM-BASICは32bit
整数型のBASICです。
また、単精度浮動小数点演算もサポートしています。
構造化プログラミング、オブジェクト指向対応も行っています。
BASICプログラムはPC等で作成し、MMCまたはSDカード経由で転送する
ことや、USBケーブルと専用ソフトを用いて転送することが可能です。
また、USBキーボードを接続し、直接プログラムを編集して実行する
ことも可能です。

動作回路やその他の詳細は、下記のWebサイトを参照してください。
http://www.ze.em-net.ne.jp/~kenken/machikania/typep.html


同梱される実行形式ファイル（uf2ファイル）は非商用利用に限り
無償で自由に利用することが可能です。
また、利用した作品を一般に公開することも可能です。その場合、
MachiKaniaシリーズを利用していることについてもWebサイトや
説明書等、いずれかの場所に記載してください。
なお、ご利用によって生じた被害や損害については責任を負いかねます。


＜machikania-p.zipファイルコンテンツ＞

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

　- shematic.png
　　MachiKania type P回路図

　- wifi.txt
　- wifi-e.txt
　　Raspberry Pi Pico Wを使ってWiFi接続を行うときのリファレンスマニュアル 


[pico_ili9341]ディレクトリ
　Raspberry Pi Pico と ILI9341 液晶の組み合わせのときに使うバイナリー
　を格納
　-phyllosoma.uf2
　　MachiKania type P BASICシステム本体（PC connect機能対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　PC connect機能により、USBケーブルで接続したPCからBASICプログラムを転送可能

　-phyllosoma_kb.uf2
　　MachiKania type P BASICシステム本体（USBキーボード接続対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　内蔵エディタとUSBキーボードでBASICプログラムを直接編集、実行可能

[pico_w_ili9341]ディレクトリ
　Raspberry Pi Pico W と ILI9341 液晶の組み合わせのときに使うバイナリー
　を格納
　-phyllosoma.uf2
　　MachiKania type P BASICシステム本体（PC connect機能対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　PC connect機能により、USBケーブルで接続したPCからBASICプログラムを転送可能
　　WiFi接続が可能

　-phyllosoma_kb.uf2
　　MachiKania type P BASICシステム本体（USBキーボード接続対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　内蔵エディタとUSBキーボードでBASICプログラムを直接編集、実行可能
　　WiFi接続が可能

[pico2_ili9341]ディレクトリ
　Raspberry Pi Pico 2 と ILI9341 液晶の組み合わせのときに使うバイナリー
　を格納
　-phyllosoma.uf2
　　MachiKania type P BASICシステム本体（PC connect機能対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　PC connect機能により、USBケーブルで接続したPCからBASICプログラムを転送可能

　-phyllosoma_kb.uf2
　　MachiKania type P BASICシステム本体（USBキーボード接続対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　内蔵エディタとUSBキーボードでBASICプログラムを直接編集、実行可能

[pico_ili9341/embed]ディレクトリ
[pico_w_ili9341/embed]ディレクトリ
[pico2_ili9341/embed]ディレクトリ
　組み込み用途などMMC/SDカードなしでもBASICプログラムを実行することが
　できるようなツール類を格納

[pcconnect]ディレクトリ
　USBケーブルを介してPC内のファイルをMachiKaniaに転送するためのツール類
　を格納

readme.txt
　このファイル

readmeLib.txt
　LIB ディレクトリー内のライブラリー説明書

＜ここより下のファイルおよびディレクトリ全てをSDカードのルートディレクトリにコピーしてください＞

MACHIKAP.INI
　MachiKania type Pの初期設定ファイル

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
MachiKania type P BASICシステム改版履歴

Phyllosoma 1.00/KM-1500（2022.8.27）
　・最初の公開バージョン

Phyllosoma 1.10/KM-1501（2022.10.1）
　・PC connect機能を追加
　・NOT#()関数を追加
　・クラス中で別のクラスを使うとコンパイルできない不具合を修正
　・クラスのスタティック関数呼び出しの不具合を修正
　・オブジェクトのフィールドに、文字列・配列を割り当てることを許可
　・一定時間後にWAVEファイルの再生が止まる不具合を修正
　・割り込み中でのWAIT・DELAYMS・DELAYUSステートメントの使用を許可
　・MUSICステートメントが一部の環境でエラーで停止する不具合を修正
　・ファイル選択画面の表示改善
　・WS2812B・STRDIM・STRD2・MAの４つのクラスを、ライブラリーに追加

Phyllosoma 1.20/KM-1502（2023.1.28）
　・USBキーボード及びエディターをサポート
　・INKEY()・READKEY()・INPUT$()の3つの関数と、INKEY割り込み機能を整備
　・クラスファイルコンパイル時に一部の環境で生じる不具合を修正
　・ALIGN4ステートメント・DATAADDRESS()関数・FUNCADDRESS()関数を追加
　・タイマー割り込みのタイミングが少しずれていたのを修正
　・SYSTEM()に、メモリーアロケーション関連の機能を追加
　・文字列で「\r」「\t」のエスケープシークエンスが使用可能に
　・OPTION CLASSCODEに対応
　・BASICプログラム実行中でのカードの抜き差しが可能に
　・ファイル選択画面でC言語で作成したHEXファイルのロードが可能に
　・CRDINI・CLDHEX・REGEXP・STRINGの４つのクラスを、ライブラリーに追加

Phyllosoma 1.30/KM-1503 (2023.9.30)
　・サンプルプログラムに、WEATHER.BAS(webページから天気予報を得る)と
　HDEAMON.BAS(web サーバー構築を構築)を追加。
　・クラスライブラリーに、HTTPD(HTTPサーバーを構築)、IR_RX(赤外線リモコン
　受信モジュールを使用)、IR_TX(赤外線LEDでリモコン信号を送信)、JSON(JSON文字列
　を解析)、WGET(指定のURLから情報を取得)を追加。
　・Raspberry Pi Pico W を用いた WiFi 接続に対応。次の命令・関数を追加： 
　　DNS$(), IFCONFIG$(), NTP, TCPACCEPT(), TCPCLIENT, TCPCLOSE, TCPRECEIVE, 
　　TCPSEND, TCPSERVER, TCPSTATUS, TLSCLIENT, WIFIERR(), WIFIERR$()
　・GCOLOR()関数の不具合を修正
　・一部USBキーボードでのキー入力不具合に対応
　・EOFの時、FGETC()関数が-1を返すようにした
　・embed用にXIAO RP2040, RP2040-Zero, and Tiny-2040に対応
　・SPIの対応ポートをINIファイルで指定出来るようにした
　・SERIALステートメントの第3引数を省略した時の不具合を修正
　・例外をトラップして画面表示するようにした
　・CORETIMERが電源投入後およそ2000秒後から不具合を起こす事を修正
　・ILI9488に対応
　・RTC(Real Time Clock)をサポート。次の命令・関数を追加： 
　　GETTIME$(), SETTIME. STRFTIME$()
　・ファイル保存時の日時設定をサポート
　・NTPサーバーによるRTCのセットをサポート
　・SYSTEM 201 呼び出しにより、ボード付属のLEDのオン・オフを出来るようにした
　・メモリーアロケーションの不具合を修正
　・FREMOVE/SETDIR等のファイル関連命令を使用時の、ガベージコレクション不具合を修正
　・WAVEプレーヤーの安定性を改善
　・クラスのスタティックメソッド呼び出し不具合を修正
　・FFINGD$(), FINFO(), FINFO$()の3つの関数を追加。ファイル一覧の作成が可能に
　・REM 文に「"」を含む際のコンパイル時の不具合を修正

Phyllosoma 1.31/KM-1504 (2023.10.28)
　・embed用のBASICプログラムで、ファイルの読み取りに対応。
　・embed用に組み込むことができるファイル数が、合計16個に増加。
　・浮動小数点の表示や文字列の扱いの際、間違った値になる不具合を修正。

Phyllosoma 1.40/KM-1505 (2024.2.17)
　・サンプルプログラムに、FILEMAN.BAS(ファイルマネージャー)を追加。
　・クラスライブラリーに、BUTTON(ボタン操作), GEN3O(源ノ角ゴシック表示), 
　　QRCODE(QRCODE表示), TSC2046(タッチパネル操作)を追加。
　・液晶を使わない場合に、SPI命令でspi1を使う事を可能にした。
　・PUTBMP命令で、横幅が長い画像を表示する際の不具合を修正。
　・ファイル一覧表示の際、ファイル更新日時の表示を可能にした。
　・ファイル一覧表示の際の、ファイル名・ファイル更新日時による表示順に対応。
　・ILI9488液晶で縦置の際の表示不具合を修正。
　・FRENAME, MKDIR命令及びFRENAME(), MKDIR()関数の追加。
　・補助コード(auxcode)を追加できる機能を実装。

Phyllosoma 1.41/KM-1506 (2024.10.06)
　・Raspberry Pi Pico 2に対応。
　・WEATHER.BASサンプルプログラムを更新
