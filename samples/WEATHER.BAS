useclass JSON,WGET
usevar Min0,Max0,Fstr0
usevar Min1,Max1,Fstr1

if val(strftime$("%Y"))<2023 then ntp

j$=WGET::FORSTRING$("https://www.recfor.net/projects/weather/tokyo.php")
REM j$=WGET::FORSTRING$("https://www.recfor.net/projects/weather/osaka.php")
o=new(JSON,j$)

Min0#=o.FQUERY#(".DailyForecasts[0].Temperature.Minimum.Value")
Max0#=o.FQUERY#(".DailyForecasts[0].Temperature.Maximum.Value")
Fstr0$=o.SQUERY$(".DailyForecasts[0].Day.IconPhrase")
Min1#=o.FQUERY#(".DailyForecasts[1].Temperature.Minimum.Value")
Max1#=o.FQUERY#(".DailyForecasts[1].Temperature.Maximum.Value")
Fstr1$=o.SQUERY$(".DailyForecasts[1].Day.IconPhrase")

print "Today is ";strftime$("%m/%d (%a)")
print "Today's forecast is ";Fstr0$
print "Minimum temperature is ";Min0#;" degree-C"
print "Maximum temperature is ";Max0#;" degree-C"
print "Tomorrow's forecast is ";Fstr1$
print "Minimum temperature is ";Min1#;" degree-C"
print "Maximum temperature is ";Max1#;" degree-C"
