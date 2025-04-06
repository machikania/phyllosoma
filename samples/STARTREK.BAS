REM /***********************************************
REM * Star Trek ver 0.3.1 for MachiKania type P/PU *
REM * Constructed by Katsumi                       *
REM * kmorimatsu@users.sourceforge.jp              *
REM * http://hp.vector.co.jp/authors/VA016157/     *
REM * The license of this program is LGPL 2.1      *
REM ************************************************
REM Strings
REM   s$ Current sector map string (64 characters)
REM     ".": None
REM     "E": Enterprise
REM     "K": Klingon starship
REM     "B": Starbase
REM     "*": Star
REM   m$ BGM
REM Dimensions:
REM   d() Damage value is between 0-100 (%)
REM     0% correspond to no damage
REM     d(0): Warp
REM     d(1): Short Range Sensor
REM     d(2): Long Range Sensor
REM     d(3): Phaser control
REM     d(4): Photon tubes
REM     d(5): Damage control
REM     d(6): Shield control
REM     d(7): Mr. Spock
REM  q(0-63) Galaxy data; y*8+x; x,y: 0-7
REM    =K*100+B*10+S, where K, B, and S are # of Klingon ships, Base, and Stars
REM  e() Enterprise data
REM    e(0), e(1): x and y positions in Galaxy
REM    e(2), e(3): x and y positions in current sector
REM    e(4): Energy
REM    e(5): # of photon tubes
REM    e(6): Shields
REM    e(7): 1 if docked to Starbase. 0 if not
REM  g() Global data
REM    g(0): Stardate in the beginning (*100)
REM    g(1): Due date for mission in stardate (*100)
REM    g(2): Current stardate (*100)
REM    g(3): Average of Klingon ship energy when appears (=200)
REM    g(4): Full energy of Enterprise when docked (=3000)
REM    g(5): # of photon tubes when docked (=10)
REM    g(6): Klingon remaining
REM    g(7): # of Klingon starhip in the beginning
REM    g(8): # of starbase in Galaxy
REM k(0-8) Klingon starship data in current sector
REM   k(0),k(3),k(6): Energy
REM   k(1),k(4),k(7): X position in current sector
REM   k(2),k(5),k(8): Y position in current sector
REM The other valiables are used temporarily and can be destroyed in sub routines.
REM */

REM Brighter blue color
palette 1,0,192,255

REM /******************************************
REM * Begin the game                          *
REM * If # of Klingon warship is more than 9, *
REM * there must be at least a starbase.      *
REM ******************************************/
print
print "¼ÅØµ ÊÞÝºÞ³¦ Æ­³Ø®¸¼Ã¸ ÀÞ»²"
print "×ÝÀÞÑÆ ·ÒÀ²ÊÞ±²Ê 0"
print "Senario #(0-999) ";
a=gosub(INPUTN,0,999)
if a<1 or 999<a then a=drawcount()%998+1
for i=0 to a:b=rnd():next
color 6:print "¼ÅØµ #";
color 4:print a;
color 6:print "¦ Ê¼ÞÒÏ½"
if 10<a then gosub INIT:goto INTRO
 REM Senarios #1-10 require at lease one starbase
 label REINIT
 gosub INIT
 if g(8)<1 then REINIT
label INTRO
REM goto SKPINT
color 5
music "Q:1/4=90"
music "L:1/24"
music "K:C"
music "G,6F9E3D2C2B,zB,_5zB,_6B,_12"
print
print "¼Þª°Ñ½ ¶°¸ À²»"
print ""
print "·ÐÊ USS ´ÝÀ°Ìß×²½ÞÉ ¾ÝÁ®³Æ"
print "´×ÊÞÚÀ"
gosub WAITM
music "G,6G9F3E2D2C2B,5zB,6B,9B,_3"
print "ÆÝÑÊ ³Á­³Ú· ";
color 7:print g(1)/100;:color 5
print " ÏÃÞÆ"
print "·ÞÝ¶ÞÆ »Ý»Þ²½Ù ¸ØÝºÞÝÃ²º¸É"
print "¼Þ­ÝÖ³¶Ý ";
color 7:print g(7);:color 5
print " ¾·¦ ¹Þ·Ê½ÙºÄ ÃÞ±Ù"
gosub WAITM
music "A,9B,3C3D3E2F2E2G12B_9A3"
print "·ÞÝ¶Þ ¾ÞÝÀ²ÃÞ ";
color 7:print g(8);:color 5
print " ¶¼®Æ"
print "Î·­³¶É³Å ·Á¦ ¼Þ­ÝËÞ¼À"
gosub WAITM
music "G6F,10z2D6C12c12"
print "¹ÝÄ³¦ ²ÉÙ"
print "","        ³Á­³Ú· ";
color 7:print g(0)/100:color 5
print "ÚÝÎß³¾²Ì Á®³¶Ý"
print
gosub WAITM
label SKPINT
color 7
gosub DOCKED:e(7)=0
a=e(1)*8+e(0)
q(a)=q(a)%1000
s$=""
gosub SRS
goto CMD

REM /********************
REM * General functions *
REM ********************/

REM Wait for specified periods (times of 1/60 sec)
label WAITT
drawcount 0
label WAIT0
if drawcount()<a then WAIT0
return

REM Wait until music ends
label WAITM
 if 1<music() then WAITM
 return

REM Current sector data
label VALSEC
 return q(e(1)*8+e(0))%1000

REM pseudo SIN*100 from A (100-899)
label SINF
 if A<500 then return 100-(A-300)*(A-300)/400
 return (A-700)*(A-700)/400-100

REM pseudo COS*100 from A (100-899)
label COSF
 if A<300 then return 100-(A-100)*(A-100)/400
 if A<700 then return (A-500)*(A-500)/400-100
 return 100-(A-900)*(A-900)/400

REM SQRT from A (0-16384); destroy B and C
label SQRTF
 B=0:C=128
 label SQRTLP
  if (B+C)*(B+C)<=A*4 then B=(B+C)/2 else C=(B+C)/2
  if C-B<2 then return B
  goto SQRTLP

REM Get piece (as ascii) in sector at x,y
label SECPC
 if x<0 or y<0 or 7<x or 7<y then return 0
 return asc(s$(y*8+x,1))

REM Show course
label SHWCRS
 print "","Î³º³¦ ¼Ã² ¼Ã¸ÀÞ»²"
 print " 4 3 2"
 print "  ";chr$(0xef);chr$(0x96);chr$(0xee)
 print " 5- -1"
 print "  ";chr$(0xee);chr$(0x96);chr$(0xef)
 print " 6 7 8"
 print "Course (1.00-8.99)?";
 return

REM Show equipments
label EQPMNT
 color 6
 if a=0 then print "Warp Engine",
 if a=1 then print "Short Range Sensor",
 if a=2 then print "Long Range Sensor",
 if a=3 then print "Phaser Control",
 if a=4 then print "Photon Tubes",
 if a=5 then print "Damage Control",
 if a=6 then print "Shield Control",
 if a=7 then print "Galaxy Map",
 color 7
 return

REM /*************
REM * Initialize *
REM *************/

label INIT
 rem clear
 dim e(9),q(63),d(7),k(8),g(8)
 m$=""
 REM Position of Enterprise
 for i=0 to 3:e(i)=rnd()/4096:next
 REM Stardate in the beginning (2100-3600)
 g(0)=((RND()/2048)+21)*10000
 REM Due date for mission in stardate (25-34 in addition)
 g(1)=g(0)+2500+(RND()/3277)*100
 REM Current stardate
 g(2)=g(0)
 REM Full energy of Enterprise when docked (=3000)
 g(4)=3000
 REM # of photon tubes when docked (=10)
 g(5)=10
 REM Average of Klingon ship energy when appears (=200)
 g(3)=200
 
 REM    g(6): Klingon remaining
 REM    g(7): # of Klingon starhip in the beginning
 REM    g(8): # of starbase in Galaxy 
 
 REM Initialize Galaxy
 REM In each sector,
 REM   Probability of 3 Klingon ships is 0.02
 REM   Probability of 2 Klingon ships is 0.03
 REM   Probability of 1 Klingon ships is 0.15
 REM   Probability of a starbase is 0.04
 REM   # of stars is between 1-8
 REM As a result, average number of total Klingon ships is 17.3.
 REM Average number of total starbase is 2.6.
 REM # of stars is between 1 and 7
 REM Less than 8 stars cannot completely cover all directions of Enterprise
 REM in center of Galaxy,less than 5 stars cannot in edge of Galaxy, and
 REM less than 3 stars cannot in corner of Galaxy.
 REM Destroy: i,j,a,b,c
 REM   a: # of Klingon starship in a sector
 REM   b: # of Starbase in a sector
 REM   c: # of Stars in a sector
 
 g(6)=0:g(7)=0:g(8)=0
 for j=0 to 7:for i=0 to 7
  a=0:b=0
  r=rnd()
  if r<6554 then a=1
  if r<1638 then a=2
  if r<655  then a=3
  if rnd()<1311 then b=1
  c=(rnd()/4682)+1
  if i<1 or 6<i or j<1 or 6<j then c=(rnd()/8192)+1
  if (i<1 or 6<i) and (j<1 or 6<j) then c=(rnd()/16384)+1
  q(j*8+i)=a*100+b*10+c+1000
  g(6)=g(6)+a
  g(7)=g(7)+a
  g(8)=g(8)+b
 next:next
 return

REM /*******************************************
REM * Initialize Enterprise                    *
REM * This is called in the beggining          *
REM * or when docked to starbase               *
REM * Damages are all zero.                    *
REM * Mr. Spock can use hospital for recovery. *
REM * Enegy will be g(4) (=3000)               *
REM * # of Photon tubes will be g(5) (=10)     *
REM * Shield will be cleared for docking       *
REM *******************************************/

label DOCKED
 for i=0 to 7:d(i)=0:next
 e(4)=g(4):e(5)=g(5):e(6)=0:e(7)=1
 return

REM /*************************************
REM * Check if docked                    *
REM * Parameters:                        *
REM *   s$: sector string                *
REM *   e(2),e(3): Enterprise sector x,y *
REM *************************************/
label CHKDOC
 a=gosub(VALSEC)%100
 e(7)=0
 if a<10 then return
 x=e(2):y=e(3):a=y*8+x
 i=0
 if 0<x and strncmp(s$(a-1),"B",1)=0 then i=1
 if x<7 and strncmp(s$(a+1),"B",1)=0 then i=1
 if 0<y and strncmp(s$(a-8),"B",1)=0 then i=1
 if y<7 and strncmp(s$(a+8),"B",1)=0 then i=1
 if i<1 then return
 music "Q:1/4=90"
 music "L:1/24"
 music "K:C"
 music "z6C6E11z1D6A11z1"
 gosub waitm
 music "G3F^3G3C3E11z1"
 a=6:gosub PEOPLE
 print "³Á­³·ÁÄ º³¼ÝÁ­³ÃÞ½"
 gosub waitm
 music "D6d11z1d6"
 print "","ÄÞ¯·Ý¸ÞÉ ·®¶¦ ´Ï¼À"
 gosub waitm
 a=5:gosub PEOPLE
 music "d^11z1A^11z1G11z1"
 print "ÄÞ¯·Ý¸Þ ¼Ï¼À"
 gosub waitm
 music "G11z1F3G3A6"
 a=d(7)
 gosub DOCKED
 if 0<a then a=2:gosub PEOPLE:print "½¯¶Ø Ö¸ÅØÏ¼À"
 return

REM /******************
REM * Command routine *
REM ******************/

label CMD
 gosub BGM
 if keys() then CMD
 REM DEBUG
 gosub WAITM
 print
 gosub SPOCK:print
 a=5:gosub PEOPLE:print "ÄÞ³¼Ï½¶?"
 print "1:NAV 2:SRS 3:LRS 4:PHA 5:TOR"
 print "6:SHE 7:DAM 8:MAP 9:RST 0:HLP ";
 i=gosub(INPUTN,0,9,1)+0x30
 if i=0x31 then gosub NAV:goto CMD
 if i=0x32 then gosub SRS:goto CMD
 if i=0x33 then gosub LRS:goto CMD
 if i=0x34 then gosub PHA:goto CMD
 if i=0x35 then gosub TOR:goto CMD
 if i=0x36 then gosub SHE:goto CMD
 if i=0x37 then gosub DAM:goto CMD
 if i=0x38 then gosub MAP:goto CMD
 if i=0x39 then gosub RST:goto CMD
 a=1:gosub PEOPLE
 print "Ò²Ú²Ø½ÄÊ Â·ÞÉ ÄµØÀÞ"
 print "1 NAV: Ü°Ìß ÄÞ×²ÌÞ"
 print "2 SRS: ÀÝ·®Ø ¾Ý»°"
 print "3 LRS: Á®³·®Ø ¾Ý»°"
 print "4 PHA: Ìª»Þ° Î³"
 print "5 TOR: º³¼ ·Þ®×²"
 print "6 SHE: ÎÞ³·Þ® ¼°ÙÄÞ"
 print "7 DAM: ÀÞÒ°¼Þ ¾²·Þ®"
 print "8 MAP: ·ÞÝ¶Þ¹² Á½Þ"
 print "9 RST: ½º¼ Ô½Ñ"
 goto CMD

REM /****************************************
REM * Mr. Spock suggests next command       *
REM * Destroy:                              *
REM *  a: # of Klingon ships in this sector *
REM *  b: # of Starbase in this sector      *
REM *  c: # of Stars in this sector         *
REM ****************************************/

label SPOCK
 if 0<d(7) then return
 a=2:gosub PEOPLE
 c=q(e(1)*8+e(0))%1000
 a=(c/100):c=c%100
 b=(c/10):c=c%10
 if 0<a then SPKKLI
  REM There is no Klingon ship in this sector
  if 0<d(3) and (0<d(4) or e(5)=0) then print "¹ÞÝ»Þ² ÌÞ·¶Þ Â¶´Ï¾Ý","","³Á­³·ÁÃÞ ¼­³Ø¼Ï¼®³":return
  if e(4)<1000 then print "ÉºØÉ ´ÈÙ·Þ°¶Þ ½¸²ÃÞ½";"","³Á­³·ÁÃÞ Î·­³¼Ï¼®³":return
  if 0<d(6) then print "¼°ÙÄÞÉ ¾²·Þ®¶Þ ÃÞ·Å²";"","¼­³Ø¦ ÏÁÏ¼®³":return
  if 0<d(1) then print "ÀÝ·®Ø¾Ý»°¶Þ º¼®³ÃÞ½","","¼­³Ø¦ ÏÁÏ¼®³":return
  print "Ã·³Á­³¾Ý¦ »¶Þ¼Ã","","¹Þ·Ê¼Ï¼®³":return
 label SPKKLI
  REM There are Klingon ships in this sector
  if e(6)<200 then print "¼°ÙÄÞ¶Þ ÖÜ¸Ã ·¹ÝÃÞ½":return
  if 0<d(3) and (0<d(4) or e(5)=0) then print "º³¹Þ· ÃÞ·Ï¾Ý","","Æ¹ÞÅ¹ÚÊÞ Ô×ÚÏ½":return
  if 0<d(6) then print "¼°ÙÄÞÉ ¾²·Þ®¶Þ ÃÞ·Å²";"","Æ¹ÞÅ¹ÚÊÞ ·¹ÝÃÞ½":return
  if 0<d(1) and a=1 and d(4)=0 and 0<e(5) and c<2 then SPKTOR
  if 0<d(1) then print "ÀÝ·®Ø¾Ý»°¶Þ º¼®³ÃÞ½","","Æ¹ÞÅ¹ÚÊÞ ·¹ÝÃÞ½":return
  if 1<a and d(3)=0 then print "Ì¸½³É Ã·Æ Õ³º³Å","",:goto SPKPHA
  if 3<c and d(3)=0 and d(4)=0 and 0<e(5) then print "Î¼É ¶½Þ¶Þ µµ½·ÞÏ½","",:goto SPKPHA
  if a=1 and d(4)=0 and 0<e(5) then SPKTOR
  if d(3)=0 then SPKPHA
  if a=3 OR e(5)<2 then print "Ã·É ¶½Þ¶Þ µµ½·ÞÏ½","","Æ¹ÞÀÎ³¶Þ Ö²ÉÃÞÊ?":return
  print "º³¼·Þ®×²ÀÞ¹ÃÞ ³Ï¸","","À²¼®ÃÞ·ÚÊÞ Ö²ÉÃÞ½¶Þ":return
 label SPKPHA
  REM * Phaser energy is divided by the distance between Enterprise and Klingon ship,       *
  REM * and multiplied by 2-3 (*=(2~3)).                                                    *
  print "Ìª»Þ°Î³¦ Â¶²Ï¼®³"
  j=0
  for i=0 to 2
   b=e(2)-k(i*3+1):c=e(3)-k(i*3+2)
   a=b*b+c*c:a=gosub(SQRTF)
   j=j+a*k(i*3)/2
  next
  print "","¼­ÂØ®¸Ê ";j;"ÎÄÞ ÃÞ½"
  return
 label SPKTOR
  print "º³¼·Þ®×²¦ Â¶²Ï¼®³"
  if d(1)=0 then return
  for i=0 to 2
   b=k(i*3+1)+1:c=k(i*3+2)+1
   if 0<k(i*3) then print "","Ã·Ê (";b;",";c;")Æ²Ï½"
  next
  return

REM /********************
REM * The other members *
REM * Parameter: a      *
REM ********************/
label PEOPLE
 if a=0 then print "Ã·¾ÂÃÞÊÅ² Ò²Ú²ÃÞ½":return
 color 5
 if a=1 then print "¶°¸ ¾ÝÁ®³:";
 if a=2 then print "Mr. ½Îß¯¸:";
 if a=3 then print "Ï¯º² ¼®³»:";
 if a=4 then print "½º¯Ä ¼®³»:";
 if a=5 then print "½°Ù° À²²: ";
 if a=6 then print "³Ì°× Á­³²:";
 if a=7 then print "ÁªºÌ ¼®³²:";
 if a=8 then print "ºÝËß­°À°: ";
 color 7
 return

REM /**********************************************
REM * Short Range Sensor                          *
REM * Parameter: s$ (Sector data string)          *
REM * Parameter: d(1) (Damage to SRS)             *
REM * Destroy: a,c,i,j,t$                         *
REM * Destroy: x, y (0-7: position of Enterprise) *
REM * Destroy: b (0-8: available range to show)   *
REM **********************************************/

label SRS
 if len(s$)<64 then gosub SRSINI
 b=0
 if d(1)<8 then b=1
 if d(1)<4 then b=2
 if d(1)<1 then b=8
 print " 12345678";:j=8:gosub SRSINF
 for j=0 to 7
  print j+1;
  for i=0 to 7
   a$=s$(j*8+i,1)
   if b<abs(i-e(2)) or b<abs(j-e(3)) then a$=chr$(0xf0):color 2
   if strncmp(a$,".",1)=0 then a$=chr$(0xa5)
   if strncmp(a$,"K",1)=0 then color 3
   if strncmp(a$,"*",1)=0 then color 6
   if strncmp(s$(j*8+i),"B",1)=0 then a$="B":color 5
   print a$;
   color 7
  next
  gosub SRSINF
 next
 return

REM Information view
label SRSINF
 if j=8 then t$="Stardate":c$=dec$(g(2)/100)
 if j=0 then t$="Mission due":c$=dec$(g(1)/100)
 if j=1 then t$="Condition"
 if j=2 then t$="Quadrant":c$=dec$(e(0)+1)+","+dec$(e(1)+1)
 if j=3 then t$="Sector":c$=dec$(e(2)+1)+","+dec$(e(3)+1)
 if j=4 then t$="Energy":c$=dec$(e(4))
 if j=5 then t$="Photon tubes":c$=dec$(e(5))
 if j=6 then t$="Shields":c$=dec$(e(6))
 if j=7 then t$="Klingon ships":c$=dec$(g(6))
 t$=t$+"        "
 color 1
 print " ";t$(0,14);
 color 7
 if j=1 and 99<gosub(VALSEC) then color 2:c$="RED"
 if j=1 and gosub(VALSEC)<100 then color 4:c$="GREEN"
 if j=1 and gosub(VALSEC)<100 and e(4)<g(4)/10 then color 6:c$="YELLOW"
 if 5<len(c$) then print c$; else print c$
 color 7
 return

REM /*****************************************************
REM * Initialize sector data                             *
REM * Parameters:                                        *
REM *   e(0),e(1): Enterprise X and Y in Galaxy          *
REM *   e(2),e(3): Enterprise X and Y in Sector          *
REM * Return:                                            *
REM *   s$: Sector data string                           *
REM * Destroy:                                           *
REM *   a, b, c: # of Klingon ships, starbase, and stars *
REM *   i,j                                              *
REM *****************************************************/
label SRSINI
 c=gosub(VALSEC)
 a=c/100
 b=((c%100)/10)
 c=c%10
 s$="."
 for i=1 to 6:s$=s$+s$:next
 i=e(3)*8+e(2):s$=s$(0,i)+"E"+s$(i+1)
 i=c:t$="*":gosub SRSISB
 i=b:t$="B":gosub SRSISB
 i=a:t$="K":gosub SRSISB
 for i=0 to 2:k(i*3)=0:next
 if a<1 then return
 REM Give power to Klingon ships
 for i=0 to a-1
  k(i*3)=(RND()+16384)*g(3)/32768
 next
 return

label SRSISB
 if i=0 then return
 j=RND()/512
 if strncmp(s$(j,1),".",1) then SRSISB
 s$=s$(0,j)+t$+s$(j+1)
 i=i-1
 if i<3 then k(i*3+1)=j%8:k(i*3+2)=(j/8)
 goto SRSISB

REM /*********************************************
REM * Long Range Sensor                          *
REM * Parameters:                                *
REM *   e(0), e(1) (Quadrant x,y: 0-7)           *
REM *   q(0-63): Quadrants data                  *
REM * destroy: a,i,j,t$,x,y                      *
REM *********************************************/

label LRS
 if 0<d(2) then a=5:gosub PEOPLE:print "º¼®³Á­³ ÃÞ½":return
 x=e(0):y=e(1)
 for i=x-1 to x+1
  if 0<=i and i<=7 then print "   ";i+1; else print "    ";
 next
 print ""
 gosub LRSTT
 for j=y-1 to y+1
  if 0<=j AND j<=7 then print j+1; else print " ";
  for i=x-1 to x+1
   print chr$(0x96);
   if i<0 or j<0 or 7<i or 7<j then print "***";: goto LRSL1
    a=q(j*8+i) % 1000
    q(j*8+i)=a
    color 3:print (a/100);: a=a % 100
    color 5:print (a/10); : a=a % 10
    color 6:print a;
    color 7
   label LRSL1
  next
  print chr$(0x96)
  if j<y+1 then gosub LRSTM
 next
 gosub LRSTB
 return

label LRSTT
 t$=chr$(0x95)+chr$(0x95)+chr$(0x95)
 print " ";chr$(0x98);t$;chr$(0x91);t$;chr$(0x91);t$;chr$(0x99)
 return

label LRSTM
 t$=chr$(0x95)+chr$(0x95)+chr$(0x95)
 print " ";chr$(0x93);t$;chr$(0x8f);t$;chr$(0x8f);t$;chr$(0x92)
 return

label LRSTB
 t$=chr$(0x95)+chr$(0x95)+chr$(0x95)
 print " ";chr$(0x9a);t$;chr$(0x90);t$;chr$(0x90);t$;chr$(0x9b)
 return

REM /**********************************
REM * Galaxy Map                      *
REM * Parameters:                     *
REM *   e(0),e(1): Quadrant x,y (0-7) *
REM *   q(0-63): Quadrants data       *
REM * Destroy: x,y,a,t$               *
REM **********************************/
label MAP
 if d(1)=0 then q(e(1)*8+e(0))=GOSUB(VALSEC)
 for x=1 to 8:print "  ";x;:next:print
 gosub MAPTT
 for y=0 to 7
  print y+1;chr$(0x96);
  for x=0 to 7
   if x=e(0) and y=e(1) then print "E"; else print " ";
   if q(y*8+x)<1000 then color 3:print (q(y*8+x)/100); else print "*";
   color 7:print chr$(0x96);
  next
  print
  print " ";chr$(0x96);
  for x=0 to 7
   if q(y*8+x)<1000 then color 5:print ((q(y*8+x)%100)/10); else print "*";
   color 7
   if q(y*8+x)<1000 then color 6:print q(y*8+x)%10; else print "*";
   color 7:print chr$(0x96);
  next
  print
  if y<7 then gosub MAPTM
 next
 gosub MAPTB
 a=1:gosub PEOPLE
 print "³Á­³·ÁÉ ¶½ÞÊ ";g(8);" ÀÞÅ";
 label MAPLP1:gosub BGM:if keys() then MAPLP1
 label MAPLP2:gosub BGM:if keys()=0 then MAPLP2
 print
 return

label MAPTT
t$=chr$(0x95)+chr$(0x95)
print " ";chr$(0x98);t$;chr$(0x91);t$;chr$(0x91);t$;chr$(0x91);t$;chr$(0x91);
print t$;chr$(0x91);t$;chr$(0x91);t$;chr$(0x91);t$;chr$(0x99)
return

label MAPTM
t$=chr$(0x95)+chr$(0x95)
print " ";chr$(0x93);t$;chr$(0x8f);t$;chr$(0x8f);t$;chr$(0x8f);t$;chr$(0x8f);
print t$;chr$(0x8f);t$;chr$(0x8f);t$;chr$(0x8f);t$;chr$(0x92)
return

label MAPTB
t$=chr$(0x95)+chr$(0x95)
print " ";chr$(0x9a);t$;chr$(0x90);t$;chr$(0x90);t$;chr$(0x90);t$;chr$(0x90);
print t$;chr$(0x90);t$;chr$(0x90);t$;chr$(0x90);t$;chr$(0x9b)
return

REM /***********************************
REM * Navigation (Warp drive)          *
REM * Parameters:                      *
REM *   s$: Sectors data               *
REM *   e(2),e(3): x,y sector position *
REM *   e(0),e(1): x,y quadrant pos.   *
REM *   e(7): docking state            *
REM * Destroy: a,b,c,i,j,t$,x,y        *
REM ***********************************/
label NAV
 a=5:gosub PEOPLE:print "Ü°Ìß ¼Ï½"
 gosub SHWCRS
 i=gosub(INPUTN,100,899,1)
 if i<100 or 899<i then gosub PEOPLE:a=0:gosub PEOPLE:return
 a=4:gosub PEOPLE
 b=90:if 0<d(0) then print "´Ý¼ÞÝÊ ¼­³ØÁ­³ ÃÞ½":b=3
 print "Ü°Ìß ¹²½³Ê ÄÞ³¼Ï½¶"
 print "Warp Factor (1-";b;")?";
 j=gosub(INPUTN,1,b,1)
 if j<1 or b<j then gosub PEOPLE:a=0:gosub PEOPLE:return
 if (e(4)-e(6)<j+10) then gosub PEOPLE:print "´ÈÙ·Þ°¶Þ ÀØÏ¾Ý":return
 REM Construct navigation vector
 a=i:x=gosub(COSF):y=-gosub(SINF)
 REM Refine navigation vector
 a=x*x+y*y:a=gosub(SQRTF)
 x=100*x/a:y=100*y/a
 REM Check if not going out galaxy
 a=e(0)*8+e(2)+(j*x/100)
 b=e(1)*8+e(3)+(j*y/100)
 if a<0 or b<0 or 63<a or 63<b then a=5:gosub PEOPLE:print "·ÞÝ¶ÞÉ ¿ÄÆ ÃÞÃ¼Ï²Ï½!";:return
 REM Move in current sector
 REM  c will valid move length
 REM  a=1 if go out from current sector
 REM  a=0 if not
 e(7)=0
 a=1:gosub PEOPLE:print "Ü°Ìß ÄÞ×²ÌÞ!"
 music "Q:1/4=90"
 music "L:1/180"
 music "K:C"
 music "C1C^1D1D^1E1F1F^1G1G^1A1A^1B1"
 gosub waitm
 music "c1c^1d1d^1e1f1f^1g1g^1a1a^1b1"
 gosub waitm
 music "B,1A,^1A,1G,^1G,1F,^1F,1E,1D,^1D,1C,^1C,1"
 gosub waitm
 c=j
 for i=1 to j
  a=e(2)+(i*x/100)
  b=e(3)+(i*y/100)
  if a<0 or b<0 or 7<a or 7<b then i=j:a=1:goto SRSSK1
   t$=s$(b*8+a,1)
   a=0
   if strncmp(t$,"E",1)=0 or strncmp(t$,".",1)=0 then SRSSK1
    a=5:gosub PEOPLE:print "¾ÞÝÎß³Æ ¼®³¶Þ²ÌÞÂ":print "","´Ý¼ÞÝ Ã²¼¼Ï½"
    a=0
    c=i-1
    i=j
  label SRSSK1
 next
 x=e(0)*8+e(2)+(c*x/100)
 y=e(1)*8+e(3)+(c*y/100)
 s$=s$(0,e(3)*8+e(2))+"."+s$(e(3)*8+e(2)+1)
 e(0)=(x/8):e(2)=x%8
 e(1)=(y/8):e(3)=y%8
 s$=s$(0,e(3)*8+e(2))+"E"+s$(e(3)*8+e(2)+1)
 REM Warp used some energy
 e(4)=e(4)-c-10
 REM Spent a day or some
 if a=0 then g(2)=g(2)+12*c else c=8:g(2)=g(2)+100
 REM Damage event. (Note that DAMNAV destroies a)
 b=a
 gosub DAMNAV
 REM Judge energy and dates
 gosub JUDGE
 REM if remaining in current sector, KLGN, SRS and return
 if b=0 then gosub CHKDOC:gosub KLGN:gosub SRS:return
 REM Enter to different sector
 a=5:gosub PEOPLE:print "Quadrant";e(0)+1;",";e(1)+1;" ÆÊ²ØÏ¼À"
 s$=""
 gosub SRS
 return

REM /*****************************
REM * Shileds                    *
REM * Parameters:                *
REM *   e(4): Total energy       *
REM *   e(6): Strength of shield *
REM *****************************/
label SHE
 a=7:gosub PEOPLE
 if 0<d(6) then print "¼°ÙÄÞÊ ¼­³ØÁ­³ ÃÞ½":return
 print "¼°ÙÄÞ·®³ÄÞ¦ ÍÝº³¼Ï½"
 print "Shieds (0-";e(4);")?";
 i=gosub(INPUTN,0,e(4),1)
 gosub PEOPLE
 if i<0 or e(4)<i then a=0:gosub PEOPLE:return
 e(6)=i
 print "·®³ÄÞ¦ ";i;"Æ ¼Ï¼À"
 return

REM /**************************************************************************************
REM * Phaser                                                                              *
REM * If shield control is damaged, phaser energy is decreased (*=0.00-0.99)              *
REM * Phaser energy equally hits each Klingon ships. For example, if 900 units are        *
REM * used for 3 Klingon ships, 300 units will hit each ship.                             *
REM * Phaser energy is divided by the distance between Enterprise and Klingon ship,       *
REM * and multiplied by 2-3 (*=(2~3)).                                                    *
REM * Phaser energy to reach Klingon ship must be more than 15% of Klingon ship's energy. *
REM * If less than that, it does not take effect.                                         *
REM * Parameters:                                                                         *
REM *   e(4),e(6): Enterprise energy and shileds values                                   *
REM *   e(2),e(3): Enterprise sector x,y position                                         *
REM *   k(i*3): Klingon energy                                                            *
REM *   k(i*3+1),k(i*3+2): Klingon sector x,y position                                    *
REM *   d(6): Shields damage                                                              *
REM * Destroy: a,b,c,i,j,x,y                                                              *
REM **************************************************************************************/
label PHA
 a=7:gosub PEOPLE
 b=e(4)-e(6)
 c=gosub(VALSEC)/100
 if 0<d(3) then print "Ìª»Þ°Ê ¼­³ØÁ­³ ÃÞ½":return
 if 0<d(1) then print "ÀÝ·®Ø¾Ý»°¶Þ Â¶´Å²ÉÃÞ";:print "","Ã·¦ È×´Ï¾Ý":return
 REM TODO: SRS damage effect will be here
 if c<1 then print "Ã·¶Þ ²Ï¾Ý":return
 print "Ìª»Þ°Î³É ¼Þ­ÝËÞ¶Þ":print "","ÃÞ·Ï¼À"
 print "Phaser (0-";b;")?";
 i=gosub(INPUTN,0,b,1)
 if i<1 or b<i then gosub PEOPLE:a=0:gosub PEOPLE:return
 if 0<d(6) then i=i*rnd()/32768:a=7:gosub PEOPLE:print "¼°ÙÄÞ¶Þ ºÝÄÛ°Ù ÃÞ·½Þ";:print "","ÊßÜ°¶Þ ";i;"Æ µÁÏ¼À!"
 e(4)=e(4)-i
 i=i/c
 music "Q:1/4=90"
 music "L:1/256"
 music "K:C"
 for j=1 to c
  gosub waitm
  music "b'1a'^1a'1g'^1g'1f'^1f'1e'1d'^1d'1c'^1c'1"
  gosub waitm
  music "b1a^1a1g^1g1f^1f1e1d^1d1c^1c1"
 next
 for j=0 to 2
  if 0<k(j*3) then gosub PHASUB
 next
 gosub KLGN
 gosub JUDGE
 return

label PHASUB
 gosub waitm
 music "c^1c1c^1c1c^1c1c^1c1c^1c1c^1c1z20"
 a=7:gosub PEOPLE
 x=k(j*3+1):y=k(j*3+2)
 b=e(2)-x:c=e(3)-y
 a=b*b+c*c:a=gosub(SQRTF)
 b=i/a*(65536+rnd())/32768
 if b<15*k(j*3)/100 then b=0
 print "Ã·(";x+1;",";y+1;")ÍÉ º³¹Þ·:";b
 k(j*3)=k(j*3)-b
 if 0<k(j*3) then return
 print "","¹Þ·Ê ¼Ï¼À"
 k(j*3)=0
 a=y*8+x
 s$=s$(0,a)+"."+s$(a+1)
 g(6)=g(6)-1
 a=e(1)*8+e(0)
 q(a)=q(a)-100
 return

REM /*****************************************
REM * Photon Torpedo                         *
REM * Parameters:                            *
REM *   e(2),e(3): Enterprise sector pos x,y *
REM *   e(5): # of photon torpedos remaining *
REM *   d(4): damage                         *
REM *   s$: sector data string               *
REM *   k(i*3): Klingon (if not active, 0)   *
REM *   k(i*3+1),k(i*3+2): Klingon pos x,y   *
REM * Destroy: i,j,a,b,c,x,y,z,t$            *
REM *****************************************/
label TOR
 a=7: gosub PEOPLE
 if e(5)<1 then print "º³¼·Þ®×²¶Þ Ó³ ±ØÏ¾Ý":return
 if 0<d(4) then print "º³¼·Þ®×²Ê ¼­³ØÁ­³ÃÞ½";:return
 print "º³¼·Þ®×²¦ ¼Þ­ÝËÞ¼Ï½"
 gosub SHWCRS
 i=gosub(INPUTN,100,899)
 if i<100 or 899<i then gosub PEOPLE:a=0:gosub PEOPLE:return
 music "Q:1/4=90"
 music "L:1/256"
 music "K:C"
 gosub waitm
 music "b'1a'1a'^1g'1g'^1f'1f'^1d'^1e'1c'^1d'1b1"
 gosub waitm
 gosub PEOPLE:print "º³¼·Þ®×²¦ Â²¾·¼Ï½":print "",
 music "c'1a1a^1g1g^1f1f^1d^1e1c^1d1B1"
 gosub waitm
 music "c1A1A^1G1G^1F1F^1D^1E1C^1D1"
 gosub waitm
 e(5)=e(5)-1
 z=-1
 for j=0 to 10
   a=i
   x=e(2)+(j*gosub(COSF)/100)
   y=e(3)-(j*gosub(SINF)/100)
   if y*8+x=z then TORL1
   if x<0 or y<0 or 7<x or 7<y then print "Ò²Á­³ ¼Ï¾Ý!":j=10:goto TORL1
     print "(";(x+1);",";(y+1);")":print "",
     a=30:gosub WAITT
     z=y*8+x
     if strncmp(s$(z),"*",1)=0 then gosub TORSTR:j=10:goto TORL1
     if strncmp(s$(z),"K",1)=0 then gosub TORKLG:j=10:goto TORL1
     if strncmp(s$(z),"B",1)=0 then gosub TORSBS:j=10:goto TORL1
     REM BGM will be here
   label TORL1
 next
 gosub KLGN
 gosub JUDGE
 return
REM Hit star
label TORSTR
 print "´ÈÙ·Þ°¶Þ Î¼Æ":print "","·­³¼­³»ÚÏ¼À!";
 return
REM Hit Klingon
label TORKLG
 music "c^1c1c^1c1c^1c1c^1c1c^1c1c^1c1"
 gosub waitm
 print "Ò²Á­³!"
 s$=s$(0,z)+"."+s$(z+1)
 a=e(1)*8+e(0)
 q(a)=q(a)-100
 g(6)=g(6)-1
 for i=0 to 2
  if k(i*3+1)=x and k(i*3+2)=y then k(i*3)=0
 next
 return
REM Hit starbase
label TORSBS
 music "c^1c1c^1c1c^1c1c^1c1c^1c1c^1c1"
 gosub waitm
 print "³Á­³·Á¦ Ê¶²¼Ã":print "","¼Ï²Ï¼À!";
 s$=s$(0,z)+"."+s$(z+1)
 a=e(1)*8+e(0)
 q(a)=q(a)-10
 g(8)=g(8)-1
 return

REM /************************************************************
REM * Klingon warships hit Enterprise                           *
REM * Hit from Klingon ship is calculated as follows:           *
REM * (Enery of Klingon; 100-300)/(distance; 1-9)*(random: 2-3) *
REM * Parameters:                                               *
REM *   k(i*3): Energy of Klingon                               *
REM *   k(i*3+1),k(i*3+2): Sector x,y position of Klingon       *
REM *   e(4),e(6): Enterprise energy and shild values           *
REM *   e(7): Docking state (1: docked; 0: not docked)          *
REM * Destroy: a,b,c,i,x,y                                      *
REM ************************************************************/
label KLGN
 for i=0 to 2
  if 0<k(i*3) then gosub KLGSUB
 next
 REM Judge shields
 gosub JUDGE
 return
label KLGSUB
 a=7:gosub PEOPLE
 x=k(i*3+1):y=k(i*3+2)
 b=e(2)-x:c=e(3)-y
 a=b*b+c*c:a=gosub(SQRTF)
 b=k(i*3)/a*(65536+rnd())/32768
 if e(7) then b=0
 print "Ã·(";x+1;",";y+1;")¶× º³¹Þ·:";b
 e(4)=e(4)-b
 e(6)=e(6)-b
 if 0<d(6) then e(6)=4*e(6)/5
 REM    If hit-score is more than 19, damage to an equipment (randomely chosen) occurs with 60% probability.
 REM    If (shield energy) is 50 times more than (hit score), this doesn't happen.
 REM    Damage value to eqipment is caluculated as: 8*(hit score)/(shield energy)+(random:1-5).
 music "Q:1/4=90"
 music "L:1/256"
 music "K:C"
 music "c^1c1c^1c1c^1c1c^1c1c^1c1c^1c1"
 gosub waitm
 if b<20 then return
 if (e(6)/b)>50 then return
 if rnd()>199661 then return
 c=(rnd()/4096)
 if c<7 then a=c:gosub EQPMNT:color 2:print "ÀÞÒ°¼Þ"
 if c=7 then a=3:gosub PEOPLE:color 2:print "Mr. ½Îß¯¸¶Þ Ì¼®³¼À!"
 music "c6z6c12z12"
 gosub waitm
 color 7
 d(c)=d(c)+(8*b/e(6))+(rnd()/8192)+1
 return

REM /*****************************
REM * Damage report              *
REM * Parameters:                *
REM *   d(0): Warp Engine        *
REM *   d(1): Short Range Sensor *
REM *   d(2): Long Range Sensor  *
REM *   d(3): Phaser control     *
REM *   d(4): Photon tubes       *
REM *   d(5): Damage control     *
REM *   d(6): Shield control     *
REM *   d(7): Mr. Spock          *
REM *****************************/
label DAM
 REM when Mr. Spock is injured, Dr. McCoy answers
 if d(7)<1 then a=2:t$="ÃÞ½" else a=3:t$="ÀÞÖ"
 gosub PEOPLE
 if 0<d(5) then print "ÀÞÒ°¼ÞºÝÄÛ°×°Ê","","¼­³ØÁ­³ ";t$:goto DAMSPK
  print "¿Ú¿ÞÚÉ ··É ÀÞÒ°¼ÞÊ","","Â·ÞÉÄµØ ";t$
  for i=0 to 6:a=i:gosub EQPMNT:print d(i):next
label DAMSPK
 if d(7)<1 then return
 a=3:gosub PEOPLE:print "Mr. ½Îß¯¸É ¹¶ÞÉ","","¼Þ®³À²Ê ",d(7);t$
 return

REM /*****************************************************************************
REM * Damage to each equipment is repaired by adding warp factor                 *
REM * (if trans-quadrant warp, 8).                                               *
REM * In addition, every warp drive, following event happens:                    *
REM *   In 8% probability, more damage to an equipment occurs: (random: 8-48)    *
REM *   In 12% probability, early repair of an equipment happens: (ramdom: 8-32) *
REM * Parameter:                                                                 *
REM *   c: warp factor                                                           *
REM * Destroy: a,i                                                               *
REM ******************************************************************************/
label DAMNAV
 music "Q:1/4=90"
 music "L:1/256"
 music "K:C"
 for i=0 to 7
  if d(i)<1 then DAMNA2
   d(i)=d(i)-c
   if 0<d(i) then DAMNA2
    d(i)=0
    if i<7 then a=i:gosub EQPMNT:color 4:print "¼­³Ø ¶ÝØ®³":color 7
    if i=7 then a=2:gosub PEOPLE:print "½¯¶Ø Ö¸ÅØÏ¼À"
    music "C6z6C12z12"
    gosub WAITM
  label DAMNA2
 next
 if 6553<rnd() then return
 c=(rnd()/4096)
 if 13107<rnd() then DAMNA3
  REM Damage to equipment
  if c<7 then a=c:gosub EQPMNT:color 2:print "º¼®³!"
  if c=7 then a=3:gosub PEOPLE:color 2:print "Mr. ½Îß¯¸¶Þ ËÞ®³·ÀÞ!"
  color 7
  d(c)=d(c)+8+(rnd()/819)
  music "c6z6c12z12"
  gosub WAITM
  return
 label DAMNA3
  REM Repaired earlier
  if d(c)<1 then return
  d(c)=d(c)-8-(rnd()/1024)
  if 0<d(c) and c<7 then a=c:gosub EQPMNT:print "¼­³Ø¶Þ ½½ÐÏ¼À":return
  if 0<d(c) and c=7 then a=3:gosub PEOPLE:print "Mr. ½Îß¯¸É ÁØ®³¶Þ","","½Þ²ÌÞÝ ½½ÝÀÞÖ":return
  d(c)=0
  if c<7 then a=c:gosub EQPMNT:color 4:print "¼­³Ø ¶ÝØ®³"
  if c=7 then a=2:gosub PEOPLE:color 4:print "½¯¶Ø Ö¸ÅØÏ¼À"
  color 7
  music "C6z6C12z12"
  gosub WAITM
  return

REM /*********************************************
REM * Win-loss judgement                         *
REM * Parameters:                                *
REM *   g(6): Total # of Klingon ships remaining *
REM *   e(4): Enterprise total energy            *
REM *   e(6): Enterprise shields strength        *
REM *   g(1): Due date for mission (stardate*100)*
REM *   g(2): Current stardate (stardate*100)    *
REM *********************************************/
label JUDGE
 if g(6)<1 then goto JUDGEW
 if e(6)<0 then print "´ÝÀ°Ìß×²½ÞÊ Ê¶²»ÚÀ":goto JUDGEL
 if e(4)<11 then a=1:gosub PEOPLE:print "´ÈÙ·Þ°¶Þ Â·Ã¼Ï¯À":goto JUDGEL
 if (g(1)/100)<(g(2)/100) then a=1:gosub PEOPLE:print "Ó³ ¼Þ¶Ý¶ÞÅ²...":goto JUDGEL
 return
label JUDGEW
 music "K:Gm"
 music "Q:1/4=150"
 music "L:1/8"
 music "z12z12z1"
 gosub waitm
 print "¸ØÝºÞÝ¶ÝÀ²Ê ¾ÞÝÒÂ¼"
 music "F4Bdc4A_gf6z"
 gosub waitm
 print "¸ØÝºÞÝÃ²º¸Ê Ü¸¾²ÚÝÎß³Æ º³Ì¸¼À"
 music "A_efgef3e3d2zdBdc9z3"
 gosub waitm
 print "µÒÃÞÄ³ ¶°¸ ¾ÝÁ®³"
 music "F4Bdc4A_gf6z"
 gosub waitm
 print "ÆÝÑÊ ½²º³»ÚÀ"
 music "A_efgef3e3d3zdBc6"
 gosub waitm
 wait 120
 end
label JUDGEL
 print "Ü¸¾²ÚÝÎß³Ê ¸ØÝºÞÝÃ²º¸Æ º³Ì¸¼À"
 print "»ÞÝÈÝÀÞ ¶°¸ ¾ÝÁ®³"
 wait 120
 end

label COMSTR:print "COMSTR":return
label COMPTD:print "COMPTD":return
label COMNAV:print "COMNAV":return


REM /***************
REM * Rest routine *
REM ****************/
label RST
 b=8+((g(1)-g(2))/100)*8
 if 99<gosub(VALSEC) then b=1:a=7:gosub PEOPLE:print "Ã·¶Þ ²Ï½!"
 a=3:gosub PEOPLE:print "ÄÞÚ¸Þ×² Ô½Ó³¶?"
 print "Periodes(1-";b;")?";
 i=gosub(INPUTN,0,b,1)
 if i<1 or b<i then a=0:gosub PEOPLE:return
 c=i
 g(2)=g(2)+(i*100/8)
 gosub DAMNAV
 if 99<gosub(VALSEC) then gosub KLGN:return
 a=3:gosub PEOPLE:print "Â¶ÚÊ ÄÚÀ¶Å?"
 gosub JUDGE
 return

REM /**************
REM * BGM routine *
REM **************/
label BGM
 if 1<music() then return
 if gosub(VALSEC)<100 then m$="":return
 if len(m$)=0 then gosub BGMSEL
 music m$(0,2)
 m$=m$(2)
 return
label BGMSEL
 REM Klingon theme
 music "Q:1/4=120"
 music "L:1/24"
 music "K:C"
 m$="G3d6G3d6G3d6G3d3G3d3G3c3F3E3B6B6z6z6"
 m$=m$+"G3d6G3d6G3d6G3d3G3d3G3c3F3B3E6E6z6z6"
 return

REM /*****************
REM * Input number   *
REM * Arguments:     *
REM *  min,max[,bgm] *
REM *****************/
label INPUTN
 var n,k,d,x,t
 do
  wait 1
  if 2<args(0) then gosub BGM
 loop while keys()
 x=system(26)
 n=args(1)
 k=-1:d=0
 do
  if 32=k then print:return n
  if 16=k then print:return -1
  if 1=k then n=n+1
  if 8=k then n=n+10
  if 2=k then n=n-1
  if 4=k then n=n-10
  if args(2)<n then n=args(2)
  if n<args(1) then n=args(1)
  cursor x,system(27)
  if args(2)=899 then
   t$=dec$(n)
   print t$(0,1);".";t$(1);
  else
   print n;" ";
  endif
  if keys()=k and 59<d then
   if 2<args(0) then gosub BGM
   wait 1
   continue
  endif
  for d=1 to 60
   if 2<args(0) then gosub BGM
   wait 1
   if keys()!=k then break
  next
  if 59<d then continue
  d=0
  do
   if 2<args(0) then gosub BGM
   k=keys():wait 1
   d=d+1
   if 15=d or k then
    cursor x,system(27)
    if args(2)=899 then
     t$=dec$(n)
     print t$(0,1);".";t$(1);
    else
     print n;" ";
    endif
   elseif 30=d then
    cursor x,system(27)
    print "    ";
    d=0
   endif
  loop until k
 loop
