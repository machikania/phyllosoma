REM RAYTRACING (Reference I/O '86/7)
USEGRAPHIC
REM SYSTEM 200,0
E=1:RESTORE DAT2
GOSUB SETPLT
DIM K(9),D#(9,3),C(9),A(9),L#(2),V#(2),O#(2)
GOSUB DTREAD
FOR Y=120 TO -119 STEP -E
 FOR X=-160 TO 159 STEP E
  Z=320
  O#(0)=0:O#(1)=0:O#(2)=0
  B#=0:Q#=0:R#=1
  S=0
  W#=SQRT#(FLOAT#(X*X+Y*Y+Z*Z))
  V#(0)=FLOAT#(X)/W#
  V#(1)=FLOAT#(Y)/W#
  V#(2)=FLOAT#(Z)/W#

LABEL L2060
  W#=-1
  FOR I=0 TO P
   GOSUB CROSS,K(I),D#(I,0),D#(I,1),D#(I,2),D#(I,3),V#(0),V#(1),V#(2)
   IF INT(T#>0) AND A(I)>=0 THEN
    GOSUB ANDSUB,I,V#(0),V#(1),V#(2)
    IF INT(T#<0) AND K(I)=1 THEN
     T#=U#
     GOSUB ANDSUB,I,V#(0),V#(1),V#(2)
    ENDIF
   ENDIF
   IF T#>0 THEN
    IF W#=-1 OR T#<W# THEN
     W#=T#
     M=I
    ENDIF
   ENDIF
  NEXT

  IF W#<0 AND R#<1 THEN JPUT
  IF W#<0 THEN CONTINUE
  O#(0)=V#(0)*W#+O#(0)
  O#(1)=V#(1)*W#+O#(1)
  O#(2)=V#(2)*W#+O#(2)
  T#=-1
  FOR I=0 TO P
   IF I=M THEN CONTINUE
   GOSUB CROSS,K(I),D#(I,0),D#(I,1),D#(I,2),D#(I,3),L#(0),L#(1),L#(2)
   IF INT(T#>0) AND A(I)>=0 THEN
    GOSUB ANDSUB,I,L#(0),L#(1),L#(2)
    IF T#>0 THEN BREAK
    IF K(I)=1 THEN
     T#=U#
     GOSUB ANDSUB,I,L#(0),L#(1),L#(2)
    ENDIF
   ENDIF
   IF T#>0 THEN BREAK
  NEXT
  IF INT(T#>0) AND C(M)!=-1 THEN B#=0.125:GOTO CHECK
  GOSUB BRIGHT
  IF C(M)!=-1 THEN NOTMIR
  IF T#<=0 THEN
   IF U#>0.85 THEN Q#=Q#+POW#(U#,20)*1.5
  ENDIF
  Q#=Q#+B#*0.1+0.0156
  IF R#<=0.125 THEN CONTINUE
  R#=R#-0.125
  GOTO L2060

LABEL NOTMIR
  IF U#>0.75 THEN Q#=Q#+POW#(U#,10)
  IF B#<0.125 THEN B#=0.125

LABEL CHECK
  IF C(M)!=0 THEN
   S=C(M)
  ELSE
   IF INT(FLOOR#(O#(0)/F#)+FLOOR#(O#(1)/F#)+FLOOR#(O#(2)/F#)) AND 1 THEN S=H ELSE S=G
  ENDIF

LABEL JPUT
  GOSUB PUTDOT,X+160,120-Y
 NEXT
NEXT
REM SYSTEM 200,1
WHILE KEYS()!=15:WEND
END

LABEL PUTDOT
VAR I,J,X,Y,A
X=ARGS(1):Y=ARGS(2)
FOR J=0 TO E-1
 FOR I=0 TO E-1
  IF FLOAT#(RND())/32768<Q# THEN
   PSET X+I,Y+J,7
  ELSE
   A=INT(B#*R#*255+FLOAT#(RND())/32768)
   GPALETTE 254,((S AND 2)>>1)*A,((S AND 4)>>2)*A,(S AND 1)*A
   PSET X+I,Y+J,254
  ENDIF
 NEXT
NEXT
RETURN

LABEL CROSS
VAR A#,B#,C#,D#,L#,M#,N#
VAR P#,Q#
A#=ARGS#(2)
B#=ARGS#(3)
C#=ARGS#(4)
D#=ARGS#(5)
L#=ARGS#(6)
M#=ARGS#(7)
N#=ARGS#(8)
IF ARGS(1)=1 THEN
 A#=O#(0)-A#:B#=O#(1)-B#:C#=O#(2)-C#
 P#=A#*L#+B#*M#+C#*N#
 Q#=P#*P#-A#*A#-B#*B#-C#*C#+D#*D#
 IF Q#<0 THEN
  T#=-1
 ELSE
  T#=SQRT#(Q#)
  U#=0-P#+T#
  T#=0-P#-T#
 ENDIF
ELSE
 P#=A#*L#+B#*M#+C#*N#
 IF P#=0 THEN
  T#=-1
 ELSE
  T#=(D#-O#(0)*A#-O#(1)*B#-O#(2)*C#)/P#
 ENDIF
ENDIF
RETURN

LABEL ANDSUB
VAR X#,Y#,Z#,F#,G#,I,J
I=ARGS(1)
X#=ARGS#(2)*T#+O#(0):Y#=ARGS#(3)*T#+O#(1):Z#=ARGS#(4)*T#+O#(2)
J=A(I)
DO WHILE J!=I
 IF K(J)=1 THEN
  F#=X#-D#(J,0):G#=F#*F#
  F#=Y#-D#(J,1):G#=F#*F#+G#
  F#=Z#-D#(J,2):G#=F#*F#+G#
  F#=D#(J,3):G#=G#-F#*F#
  IF G#>0 THEN T#=-1
 ELSE
  IF D#(J,0)*X#+D#(J,1)*Y#+D#(J,2)*Z#>D#(J,3) THEN T#=-1
 ENDIF
 IF T#=-1 THEN BREAK
 J=A(J)
LOOP
RETURN

LABEL BRIGHT
VAR X#,Y#,Z#,T#
IF K(M)=1 THEN
 X#=O#(0)-D#(M,0)
 Y#=O#(1)-D#(M,1)
 Z#=O#(2)-D#(M,2)
 T#=SQRT#(X#*X#+Y#*Y#+Z#*Z#)
 X#=X#/T#:Y#=Y#/T#:Z#=Z#/T#
ELSE
 X#=D#(M,0):Y#=D#(M,1):Z#=D#(M,2)
ENDIF
B#=L#(0)*X#+L#(1)*Y#+L#(2)*Z#
IF V#(0)*X#+V#(1)*Y#+V#(2)*Z#>0 THEN B#=0-B#
IF B#<0 THEN B#=0
T#=2*(V#(0)*X#+V#(1)*Y#+V#(2)*Z#)
V#(0)=V#(0)-T#*X#
V#(1)=V#(1)-T#*Y#
V#(2)=V#(2)-T#*Z#
U#=V#(0)*L#(0)+V#(1)*L#(1)+V#(2)*L#(2)
RETURN

LABEL SETPLT
VAR I
FOR I=1 TO 6
PALETTE I+8,(I AND 2)*16,(I AND 4)*8,(I AND 1)*32
NEXT
RETURN

LABEL DTREAD
VAR I,J,N,X#,Y#,Z#,T#
L#(0)=VAL#(READ$())
L#(1)=VAL#(READ$())
L#(2)=VAL#(READ$())
F#=VAL#(READ$())
G=READ():H=READ()
I=0:J=-1
WHILE 1
 N=READ()
 IF N=0 THEN
  IF J>=0 THEN
   BREAK
  ELSE
   J=I-1:CONTINUE
  ENDIF
 ENDIF
 K(I)=N
 X#=FLOAT#(READ())
 Y#=FLOAT#(READ())
 Z#=FLOAT#(READ())
 D#(I,3)=FLOAT#(READ())
 C(I)=READ()
 A(I)=READ()-1
 IF K(I)=2 THEN
  T#=SQRT#(X#*X#+Y#*Y#+Z#*Z#)
  X#=X#/T#:Y#=Y#/T#:Z#=Z#/T#
  D#(I,3)=D#(I,3)/T#
 ENDIF
 D#(I,0)=X#
 D#(I,1)=Y#
 D#(I,2)=Z#
 I=I+1
WEND
P=J
RETURN

LABEL DAT1
DATA "-0.57735"
DATA "0.57735"
DATA "-0.57735"
DATA "101"
DATA 4,5
DATA 1,  80,  0,400,  100,6,2
DATA 2,  -5,  3, -5,-2150,6,1
DATA 1,  80,  0,400,   60,2,0
DATA 1,-100,-50,400, 100,-1,0
DATA 2,   0,  0,  1,  520,0,0
DATA 0
DATA 0

LABEL DAT2
DATA "-0.57735"
DATA "0.57735"
DATA "-0.57735"
DATA "101"
DATA 4,5
DATA 1,   0,  73,715,100,-1,0
DATA 1,   0,-100,600,100, 6,0
DATA 1,-100,-100,733,100, 1,0
DATA 1, 100,-100,773,100, 2,0
DATA 2,   0,   1,  0,-200,0,0
DATA 0
DATA 0

LABEL DAT3
DATA "-0.57735"
DATA "0.57735"
DATA "-0.57735"
DATA "0"
DATA 0,0
DATA 1,80,0,400,100,5,2
DATA 0
DATA 2,-5,3,-5,-2150,5,1
DATA 0
