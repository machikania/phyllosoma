REM Initialize
n=4:s=0

REM Main loop
do
  cursor 0,5
  print "Select by Up/Down key"
  print "and hit Fire key";
  cursor 0,10
  print "    Star Trek"
  print "    Star Trek 2"
  print "    Mozart"
  print "    Bach"
  s=gosub(CHOICE)
  cls
  if s=0 then gosub STREK1
  if s=1 then gosub STREK2
  if s=2 then gosub PAPAG
  if s=3 then gosub BRAND
loop

REM Select routine
label CHOICE
do
  do:loop while keys()
  cursor 2,10+s
  print ">";
  wait 2
  do
    k=keys()
  loop until k
  wait 2
  cursor 2,10+s
  print " ";
  if k=16 OR k=32 then return s
  if k=1 then s=s-1
  if k=2 then s=s+1
  if s<0 then s=0
  if s=n then s=n-1
loop

REM Wait until music ends
label WAITM
do:loop while 1<music()
return

REM Star Trek season 1
label STREK1
music "Q:1/4=90"
music "L:1/24"
music "K:C"
music "G,6F9E3D2C2B,zB,_5zB,_6B,_12"
gosub waitm
music "G,6G9F3E2D2C2B,5zB,6B,9B,_3"
gosub waitm
music "A,9B,3C3D3E2F2E2G12B_9A3"
gosub waitm
music "G6F,6z3A,4F4A4c12"
return

REM Star Trek The Next Generation
label STREK2
music "K:Gm"
music "Q:1/4=150"
music "L:1/8"
music "F4Bdc4A_gf6zA_efgef3e3d2zdBdc9z3"
gosub waitm
music "F4Bdc4A_gf6zA_efgef3e3d3zdBc6"
return

REM Mozart Die Zauberflote
label PAPAG
music "K:C"
music "Q:1/4=225"
music "L:1/8"
music "edczczdcBcdzezdzdBG2"
gosub WAITM
music "zGg2zgezdzcz"
gosub WAITM
music "cedcBcdcBcdzez"
gosub WAITM
music "d2zdg2zgd2zdBzAzGz"
gosub WAITM
music "GBdzdzedcdezdzcz"
gosub WAITM
music "gzgB2zBdcBcdzezdzBzcz"
return

REM BACH Brandenburg Concertos
label BRAND
music "Q:1/4=90"
music "K:C"
music "L:1/128"
music "C7zC7zE7zE7zG7zG7zc7zc7zB8c8B8A8G8F8E8D8"
gosub WAITM
music "C7zC7zE7zE7zG7zG7zc7zc7zd8z8G8z24"
gosub WAITM
music "d7zd7ze7ze7zd7zd7zf7zf7ze7ze7z"
gosub WAITM
music "a7za7zg7zg7zB7zB7zc7zc7z"
gosub WAITM
music "F7zF7zE7zE7zA7zA7zG7zG7z"
gosub WAITM
music "B,7zB,7zC7zC7zG,7zG,7zF7zF7z"
gosub WAITM
music "E8D8E8C8D8z8zG,8z8"
gosub WAITM
music "C8D8E8F8G8A8B8G8"
gosub WAITM
music "c7zc7zB7zB7zd7zd7zc7zc7zf7zf7ze7ze7z"
gosub WAITM
music "a7za7zg7zg7z"
gosub WAITM
music "f8e8d8c8d8z8G8A8B_8G8A8c8B8d8"
gosub WAITM
music "G8F8E8C8D8F8E8G8C8B,8A,8F8"
gosub WAITM
music "D4E4D4E4D4E4D4E4D4E4D8z8C7zC16"
return
