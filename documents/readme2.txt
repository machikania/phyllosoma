同梱される実行形式ファイル（uf2ファイル）は非商用利用に限り
無償で自由に利用することが可能です。
また、利用した作品を一般に公開することも可能です。その場合、
MachiKaniaシリーズを利用していることについてもWebサイトや
説明書等、いずれかの場所に記載してください。
なお、ご利用によって生じた被害や損害については責任を負いかねます。


＜machikania-p2.zipファイルコンテンツ＞

[pico_ili9488]ディレクトリ
　Raspberry Pi Pico と ILI9488 液晶の組み合わせのときに使うバイナリー
　を格納
　-phyllosoma.uf2
　　MachiKania type P BASICシステム本体（PC connect機能対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　PC connect機能により、USBケーブルで接続したPCからBASICプログラムを転送可能

　-phyllosoma_kb.uf2
　　MachiKania type P BASICシステム本体（USBキーボード接続対応版）
　　PCとRaspberry Pi PicoをUSB接続し書き込む
　　内蔵エディタとUSBキーボードでBASICプログラムを直接編集、実行可能

[pico_w_ili9488]ディレクトリ
　Raspberry Pi Pico W と ILI9488 液晶の組み合わせのときに使うバイナリー
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

[pico_ili9488/embed]ディレクトリ
[pico_w_ili9488/embed]ディレクトリ
　組み込み用途などMMC/SDカードなしでもBASICプログラムを実行することが
　できるようなツール類を格納

[xiao_embed]ディレクトリ
　Seeed XIAO RP2040への組み込み用途で使うバイナリーを格納
　-phyllosoma.uf2
　　MachiKania type P BASICシステム本体（USBキーボード接続非対応版）
　　PCとSeeed XIAO RP2040をUSB接続し書き込む
　　convert.phpにより、MACHIKAP.BASなどを埋め込む

　-phyllosoma_kb.uf2
　　MachiKania type P BASICシステム本体（USBキーボード接続対応版）
　　PCとSeeed XIAO RP2040をUSB接続し書き込む
　　convert.phpにより、MACHIKAP.BASなどを埋め込む
　　BASICプログラム中でUSBキーボードを使用可能
