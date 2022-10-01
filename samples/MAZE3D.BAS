REM **********************
REM * 3D Maze by Katsumi *
REM *   for MachiKania   *
REM *      ver 0.2       *
REM **********************
REM The size of 3D view is 24 x 24
REM It's shown in (1,1)-(24,24)
REM There are free lines: 0, and 25-26
REM There are free columns: 0, and 25-29
REM Variables:
REM  a-n: local variables
REM  x,y: current position
REM  q,r: vector
REM  w:   parameter for current view

REM Initialize
dim m(29*27)
gosub INIPCG

REM construt a maze
label START
for i=0 to 29*27-1:m(i)=0:next
color 7:cls

REM Construct side walls
for x=0 to 28
 a=x:b=0:gosub PBLOCK
 a=x:b=26:gosub PBLOCK
next
for y=0 to 26
 a=0:b=y:gosub PBLOCK
 a=28:b=y:gosub PBLOCK
next
REM Construct center pins and connections
for y=2 to 24 step 2:for x=2 to 26 step 2
 a=x:b=y:gosub PBLOCK
 label RAGAIN
 r=rnd()%4
 if r=0 AND 15<m(x+y*29-29) then RAGAIN
 if r=1 AND 15<m(x+y*29+29) then RAGAIN
 if r=2 AND 15<m(x+y*29-1) then RAGAIN
 if r=3 AND 15<m(x+y*29+1) then RAGAIN
 if r=0 then a=x:b=y-1
 if r=1 then a=x:b=y+1
 if r=2 then a=x-1:b=y
 if r=3 then a=x+1:b=y
 gosub PBLOCK
 if gosub(MCHECK)=0 then MOK
  if r=0 then m(x+y*29-29)=0:cursor x,y-1:print " ";
  if r=1 then m(x+y*29+29)=0:cursor x,y+1:print " ";
  if r=2 then m(x+y*29-1)=0:cursor x-1,y:print " ";
  if r=3 then m(x+y*29+1)=0:cursor x+1,y:print " ";
  goto RAGAIN
  REM Make maze from the beginning
  x=26:y=24
 label MOK
next:next
gosub CLRCOR
cls

REM Initialize game
x=1:y=1:q=1:r=0:t=0:drawcount 0

label MAINLP

REM Create view data in w
j=1:w=0
for i=0 to 7
  c=x+i*q+(y+i*r)*29
  if m(c) then d=2 else d=0
  gosub LEFT
  if m(c+a+b*29) then d=d+1
  gosub RIGHT
  if m(c+a+b*29) then d=d+4
  w=w+j*d
  j=j*16
  a=x+(i+1)*q:b=y+(i+1)*r
  if a<0 OR 28<a OR b<0 OR 26<b then i=7
next

REM Show the view
gosub CLRSCR
gosub VIEW
gosub STGL
if x=27 AND y=25 then GOAL

REM Key check and move/rotate
label KEYIN
if 59<drawcount() then drawcount drawcount()-60:t=t+1:gosub PTIME
if keys() then KEYIN
label KEYIN2
if 59<drawcount() then drawcount drawcount()-60:t=t+1:gosub PTIME
k=keys()
if k=0 then KEYIN2
REM if k=32 then START
a=q:b=r
if k=4 then gosub LEFT
if k=8 then gosub RIGHT
q=a:r=b
if k=1 and m(x+q+(y+r)*29)=0 then x=x+q:y=y+r
if k=2 and m(x-q+(y-r)*29)=0 then x=x-q:y=y-r
goto MAINLP

REM Rotate vector left
label LEFT:a=r:b=-q:return

REM Rotate vector right
label RIGHT:a=-r:b=q:return

label GOAL
cursor 0,10:color 5
print "  {||}  {||}  {||}  |"
print "  |  |  |  |  |  |  |"
print "  |     |  |  |  |  |"
print "  | [|  |  |  ||||  |"
print "  [||]  [||]  |  |  ||||"
print
print "    Press START button"
print "   to explore next maze"
label GOAL2
if keys(16) then START
goto GOAL2

REM Walk from x-1,y-1 to 1,1
label MCHECK
gosub CLRCOR
a=x-1:b=y-1
label MC1
 if a=1 AND b=1 then return 0
 m(a+b*29)=m(a+b*29)+1
 if 47<m(a+b*29+1)+m(a+b*29-1)+m(a+b*29+29)+m(a+b*29-29) then m(a+b*29)=15
 e=m(a+b*29-1):c=a-1:d=b
 f=m(a+b*29-29):if f<e then c=a:d=b-1:e=f
 f=m(a+b*29+29):if f<e then c=a:d=b+1:e=f
 f=m(a+b*29+1) :if f<e then c=a+1:d=b:e=f
 if 10<e then return 1
 a=c:b=d
goto MC1

REM Print maze
label PMAZE
cls
for b=0 to 26:for a=0 to 28
 cursor a,b
 i=m(a+b*29)
 if i=0 then print " ";
 if i>15 then print "*";
next:next
return

REM Clear Corridor
label CLRCOR
for i=0 to 29*27-1
 if m(i)<16 then m(i)=0
next
return

label PBLOCK
cursor a,b:print "*";
m(a+b*29)=16
return

REM   @@@ -- bits 28-30
REM   @@@ -- bits 24-26
REM   @@@ -- bits 20-22
REM   @@@ -- bits 16-18
REM   @@@ -- bits 12-14
REM   @@@ -- bits  8-10
REM   @@@ -- bits  4-6
REM   @X@ -- bits  0-2
REM    |
REM    +---- You are here and watching up


REM VIEW routine
label VIEW
restore VIEWD
label VIEW0
  if NOT(w AND 0x20) then VIEW1
    a=w%16
    if 0<a then gosub SKIPD
    if 1<a then gosub SKIPD
    if 4<a then gosub SKIPD
    gosub DRAW
    return
  label VIEW1
    gosub SKIPD:gosub SKIPD:gosub SKIPD:gosub SKIPD
    if (w%16) AND 0x01 then gosub DRAW:gosub SKIPD else gosub SKIPD:GOSUB DRAW
    if (w%16) AND 0x04 then gosub DRAW:gosub SKIPD else gosub SKIPD:GOSUB DRAW
  w=(w/16)
if 16<w then VIEW0
return

REM DRAW routine
REM a:font b:increment c:last position d:first position
label DRAW
d=read()
if d=0 then return
a=0x80+d%100:d=(d/100)
b=d%100:d=(d/100)
c=d%100
d=(d/100)
i=d
do 
  cursor i%30,i/30:print chr$(a)
  if i=d+c*b then break
  i=i+b
loop

goto DRAW

REM Skip data
label SKIPD
if read() then SKIPD
return

label CLRSCR
cursor 1,0:color 6:print "Pos:";:color 7:print x;",";y;" ";
gosub PTIME
for i=1 to 24
 print "                          "
next
cursor 0,1
return

label PTIME
cursor 15,0:color 5:print "Time:";:color 7:print t;" "
return

REM Show start and goal
label STGL
for i=0 to 5
  a=x+i*q+(y+i*r)*29
  if a=30 then color 6:gosub STGL0
  if a=752 then color 5:gosub STGL0
  if m(a) then i=5
next
color 7
return
label STGL0:if 0<i then STGL1
  cursor 2,23:print "{||||||||||||||||||||}";
  cursor 1,24:print "{||||||||||||||||||||||}";
  return
label STGL1:if 1<i then STGL2
  cursor 5,20:print "{||||||||||||||}";
  cursor 4,21:print "{||||||||||||||||}";
  cursor 3,22:print "{||||||||||||||||||}";
  return
label STGL2:if 2<i then STGL3
  cursor 7,18:print "{||||||||||}";
  cursor 6,19:print "{||||||||||||}";
  return
label STGL3:if 3<i then STGL4
  cursor 9,16:print "{||||||}";
  cursor 8,17:print "{||||||||}";
  return
label STGL4:if 4<i then STGL5
  cursor 10,15:print "{||||}";
  return
label STGL5
  cursor 11,14:print "{||}";
  return

label INIPCG
usepcg
pcg 0x80,0xff808080,0x80808080
pcg 0x81,0xff010101,0x01010101
pcg 0x82,0x80808080,0x808080ff
pcg 0x83,0x01010101,0x010101ff
pcg 0x84,0x80808080,0x80808080
pcg 0x85,0x01010101,0x01010101
pcg 0x86,0xff000000,0x00000000
pcg 0x87,0x00000000,0x000000ff
pcg 0x88,0x80402010,0x08040201
pcg 0x89,0x01020408,0x10204080
pcg 0x8a,0x00000000,0xf8141211
pcg 0x8b,0x111214f8,0x00000000
pcg 0x8c,0x00000000,0x1f284888
pcg 0x8d,0x8848281f,0x00000000
pcg 0x8e,0x00000000,0xff000000
pcg 0x8f,0x000000ff,0x00000000
pcg 0x90,0x80402010,0x0f080808
pcg 0x91,0x0808080f,0x10204080
pcg 0x92,0x01020408,0xf0101010
pcg 0x93,0x101010f0,0x08040201
pcg 0x7b,0x0103070f,0x1f3f7fff
pcg 0x7c,0xffffffff,0xffffffff
pcg 0x7d,0x80c0e0f0,0xf8fcfeff
pcg 0x5b,0xff7f3f1f,0x0f070301
pcg 0x5d,0xfffefcf8,0xf0e0c080
return

REM 3D view-drawing data
REM    +----------- first position
REM    | +--------- # of characters
REM    | | +------- increment value
REM    | | | +----- font # (+0x80)
REM    | | | |
REM  091230106

label VIEWD

REM 1st line
REM Front closed, left open, right open
data 091230106,661230107,0
REM Front closed, left closed, right open
data 093210106,663210107,031013108,692012909,092193005,0
REM Front closed, left open, right closed
data 091210106,661210107,054012909,713013108,113193004,0
REM front closed, lect closed, right closed
data 093190106,663190107,031013108,692012909,092193005,054012909,713013108,113193004,0
REM front open, left closed
data 031010008,062010008,721010009,692010009,0
REM front open, left open
data 091010006,092010001,122173005,661010007,662010003,0
REM front open, right closed
data 054010009,083010009,713010008,744010008,0
REM front open, right open
data 113010000,114010006,143173004,683010002,684010007,0

REM 2nd line
data 183190106,573190107,092193005,113193004,0
data 093023108,605022909,185133005,186160106,576160107,113193004,0
data 183160106,573160107,092193005,112010009,141010009,170010009,620010008,651010008,682010008,200133004,0
data 093023108,605022909,185133005,186130106,576130107,200133004,112022909,620023108,0
data 093023108,605022909,0
data 092193005,183010006,184010006,185010001,215113005,573010007,574010007,575010003,0
data 112010009,141010009,170010009,620010008,651010008,682010008,0
data 113193004,200010000,201010006,202010006,230113004,590010002,591010007,592010007,0

REM 3rd line
data 246130106,516130107,185133005,200133004,0
data 248110106,518110107,247093005,200133004,186013108,547012909,0
data 246110106,516110107,185133005,258093004,199012909,558013108,0
data 248090106,518090107,199012909,247093005,258093004,558013108,186013108,547012909,0
data 186010008,217010008,547010009,576010009,0
data 185133005,246010006,247010001,277073005,516010007,517010003,0
data 199010009,228010009,558010008,589010008,0
data 200133004,258010000,259010006,288073004,528010002,529010007,0

REM 4th line
data 308090106,458090107,247093005,258093004,0
data 310070106,460070107,309053005,258093004,248013108,489012909,0
data 308070106,458070107,247093005,316053004,257012909,496013108,0
data 310050106,460050107,309053005,316053004,248013108,489012909,257012909,496013108,0
data 248013108,489012909,0
data 247093005,308010006,309010001,339033005,458010007,459010003,0
data 257010009,286010009,496010008,527010008,0
data 258093004,316010000,317010006,346033004,466010002,467010007,0

REM 5th line
data 340050106,430050107,309053005,316053004,0
data 341040106,431040107,340033005,316053004,310010008,460010009,0
data 340050106,430050107,309053005,345033004,315010009,465010008,0
data 341040106,431040107,340033005,345033004,310010008,460010009,315010009,465010008,0
data 310010008,460010009,0
data 309053005,340010001,370013005,430010003,0
data 315010009,465010008,0
data 316053004,345010000,375013004,435010002,0

REM 6th line
data 371030106,401030107,340033005,345033004,0
data 372020106,402020107,371013005,345033004,341010008,431010009,0
data 371020106,401020107,340033005,374013004,344010009,434010008,0
data 372010106,402010107,371013005,374013004,341010008,431010009,344010009,434010008,0
data 341010008,431010009,0
data 340033005,371010001,401010003,0
data 344010009,434010008,0
data 345033004,374010000,404010002,0

REM 7th line
data 372010114,402010115,371013005,374013004,0
data 373010014,403010015,374013004,372010016,402010017,0
data 372010014,402010015,371013005,373010018,403010019,0
data 372010016,402010017,373010018,403010019,0
data 372010008,402010009,0
data 371010005,372010010,401010005,402010011,0
data 373010009,403010008,0
data 373010012,374010004,403010013,404010004,0
