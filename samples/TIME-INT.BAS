REM Timer Interrupt Demo
USETIMER 1000000:REM 1sec(1,000,000us)
INTERRUPT TIMER,TISR

T=0

REM Wait for timer interrupt
DO
 IDLE
LOOP

REM Timer Interrupt Service Routine
LABEL TISR
 T=T+1
 CURSOR 0,10
 PRINT T
RETURN
