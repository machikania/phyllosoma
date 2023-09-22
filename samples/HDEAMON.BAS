useclass HTTPD
h=new(HTTPD,80,"/httproot")
print "server started as http://";ifconfig$(0);"/"
do
  h.START()
  system 201,val(h.GETPARAM$("led"))
  print h.LASTURI$()
loop


REM 次の内容を、"index.htm"のファイル名で保存し、
REM "httproot"ディレクトリーに配置する。
REM ただし、各行冒頭の「REM 」は、削除すること。

REM <html><head><title>Test</title>
REM <meta name="viewport" content="width=1, initial-scale=5"></head>
REM <body>
REM <h5 style="text-align:center"><a href="?led=1">ON</a></h5>
REM <h5 style="text-align:center"><a href="?led=0">OFF</a></h5>
REM </body></html>
