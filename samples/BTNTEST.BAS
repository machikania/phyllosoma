usegraphic
useclass TBUTTON,GEN3O

REM for RP2350-Touch-LCD-2
useclass CST816D
T=new(CST816D)

REM for ILI9341 + TSC2046
REM useclass TSC2046
REM T=new(TSC2046)

REM for ResTouch
REM useclass TSC2046
REM T=new(TSC2046,9,10)

K=new(GEN3O,88,"UTF-8")

for y=0 to 1
  for x=0 to 4
    point x*60,40+y*100
    K.GPRT(chr$(0x30+x+y*5),0,7)
    new(TBUTTON,x*60,40+y*100,48,88,funcaddress(push_cb),funcaddress(release_cb),x+y*5)
  next
next

do
  wait 1
  if T.POLLING() then
    TBUTTON::touch(T.Xpos, T.Ypos)
  else
    TBUTTON::release()
  endif
loop

label push_cb
  print args(1);
  point args(4),args(5)
  K.GPRT(chr$(0x30+args(1)),0,3)
return

label release_cb
  point args(4),args(5)
  K.GPRT(chr$(0x30+args(1)),0,7)
return
