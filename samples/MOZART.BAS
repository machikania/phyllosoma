do while inkey()+keys():loop

playwave "mozart.wav"

do while playwave()
  wait 1
loop until inkey()+keys()
