＜MachiKania ライブラリーについて＞

LIBディレクトリには幾つかのサブディレクトリーがあり、それぞれのディレクトリー
内にクラスファイルがおかれています。MachiKaniaでこれらのクラスを使うには、
ディレクトリー群を、「LIB」という名のディレクトリーに収めて、SD/MMCカードの
ルートに置いて下さい。これにより、USECLASSステートメントで宣言することにより、
これらのクラスを使うことが可能になります。

このライブラリーに含まれているクラスの（不完全かもしれない）一覧表は、以下の通り
です。それぞれのクラスの使い方については、それぞれのドキュメント（help.txt等）を
参照して下さい。

BIGNUM
	小数点以下1000桁までの長倍数演算を行うためのクラス。

BUTTON
	上下左右、START、FIREボタンを使いやすくするためのクラス。

C24LC
	EEPROM 24LC512読み書きのためのクラス。

CIOEX
	I/Oエキスパンダー、MCP23017を制御するためのクラス。

CKNJ8
	日本語表示クラス。美咲フォント(8x8)を使用。

CKNJ12
	日本語表示クラス。東雲フォント(12x12)を使用。

CKNJ16
	日本語表示クラス。東雲フォント(16x16)を使用。

CLDHEX
	HEXファイルの内容をメモリーに取り込むためのクラス。

CRDINI
	MachiKania INIファイル（MACHIKAP.INI、MACHIKAM.INI等）を調査するためのクラス。

CSWTIF
	TIFF画像表示クラス。

GEN3O
	29から118ピクセルの大きなフォント、源ノ角ゴシックを表示するためのクラス。machikap-p2-xxx.zipに含まれる。

HTTPD
	Wifi接続時に、HTTPサーバーを構築するためのクラス。

IR_RX
	赤外線リモコン受信モジュールを使用するためのクラス。

IR_TX
	赤外線LEDでリモコン信号を送信するためのクラス。

JSON
	JSON文字列を解析するためのクラス。

MA
	メモリーアロケーションの為のクラス。STRDIMで使用。

QRCODE
	画面にQRCODEを表示するためのクラス。

REGEXP
	MachiKania type Pで正規表現を使うためのクラス。

STRD2
	文字列を含む配列を使用するためのクラス（短い文字列を多数含む配列を定義するときに使用）。

STRDIM
	文字列を含む配列を使用するためのクラス（長い文字列や可変長文字列を含む配列を定義するときに使用）。

STRING
	JavaのStringクラス様の、文字列オブジェクトを扱うクラス。

TSC2046
	LCDタッチパネル(TSC2046)からの情報を取得するためのクラス。

WGET
	Wifi接続時に、クライアントとして指定のURLから情報を取得するためのクラス。

WS2812B
	シリアル接続LED WS2812Bを利用するためのクラス。

------------------------------------------------------------------------
MachiKania type P クラスライブラリー改版履歴

Phyllosoma 1.00/KM-1500（2022.8.27）
　・最初の公開バージョン
　・BIGNUM, C24LC, CIOEX, CKNJ8, CKNJ12, CKNJ16, CSWTIFを含む

Phyllosoma 1.10/KM-1501（2022.10.1）
　・MA, STRDIM,STRD2, WS2812Bを追加
　・CSWTIF ver 0.3

Phyllosoma 1.20/KM-1502（2023.1.28）
　・CLDHEX, CRDINI, REGEXP, STRINGを追加

Phyllosoma 1.30/KM-1503 (2023.9.30)
　・HTTPD, IR_RX, IR_TX, JSON, WGETを追加
　・CKNJ8 ver 0.4
　・CKNJ12 ver 0.4
　・CKNJ16 ver 0.4

Phyllosoma 1.31/KM-1504 (2023.10.28)
　・変更なし

Phyllosoma 1.40/KM-1505 (2024.2.17)
　・BUTTON, QRCODE, TS2046を追加
　・CKNJ8 ver 0.5
　・CKNJ12 ver 0.5
　・CKNJ16 ver 0.5
　・HTTPD ver 0.3.2
