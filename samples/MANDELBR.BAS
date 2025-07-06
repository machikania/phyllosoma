REM Mandelbrot set for MachiKania

usegraphic
usevar X_MIN,X_MAX,Y_MIN,Y_MAX,MAX_ITER

W = SYSTEM(22)
H = SYSTEM(23)
X_MIN# = -2.0
X_MAX# = 1.0
Y_MIN# = -1.5
Y_MAX# = 1.5

MAX_ITER = 16

usevar zx,zy,cx,cy,zx_next,iter

REM zx As FLOAT, zy As FLOAT
REM cx As FLOAT, cy As FLOAT
REM zx_next As FLOAT
REM iter As INTEGER
REM t As INTEGER
REM x As INTEGER, y As INTEGER
REM g As INTEGER

for i=0 to 255
 gpalette i,i,i,i
next

For y = 0 To H - 1
 cy# = Y_MIN# + (Y_MAX# - Y_MIN#) * float#(y) / float#(H)
 For x = 0 To W - 1
  cx# = X_MIN# + (X_MAX# - X_MIN#) * float#(x) / float#(W)
  zx# = 0
  zy# = 0
  iter = 0
  t = int(zx# * zx# + zy# * zy#)

  Do While t <= 4 And iter < MAX_ITER
   zx_next# = zx# * zx# - zy# * zy# + cx#
   zy# = 2 * zx# * zy# + cy#
   zx# = zx_next#
   iter = iter + 1

   If zx#*zx#>4 Or zy#*zy#>4 Then break
  Loop

  g = 255 * iter / MAX_ITER
  pset x,y,g

 Next
Next

gpalette 7,255,255,255
do:wait 1:loop until inkey()+keys()
End
