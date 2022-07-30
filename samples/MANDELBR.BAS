WIDTH 80:USEVAR CA,CB,FLAG
FOR Y=-13 TO 13
  FOR X=-39 TO 39
    CA#=FLOAT#(X)*0.0458
    CB#=FLOAT#(Y)*0.08333
    A#=CA#
    B#=CB#
    FLAG=0
    FOR I=0 TO 15
      T#=A#*A#-B#*B#+CA#
      B#=2*A#*B#+CB#
      A#=T#
      IF (A#*A#+B#*B#)>4 THEN
        FLAG=1
        BREAK
      ENDIF
    NEXT
    IF FLAG=0 THEN
      PRINT " ";
    ELSE
      IF I>9 THEN I=I+7
      PRINT CHR$(48+I);
    ENDIF
  NEXT
  PRINT
NEXT
A$=INPUT$()