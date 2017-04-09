#!/usr/bin/ruby
require 'date'

outputname=Date.today.to_s.gsub('-','')
outputname="pp#{outputname}.csv"

op = File.open(outputname,'w')
printf(op,"%%<starttime>0\n")
printf(op,"%% Seconds, Power, HR, Cad,Speed(kph), Dist(m),Alt(m),Temp(C),Torq(nm)\n")

seconds=0.0                     # this will be time of each line
currentTime=0.0                 # this will be the time offset for the entire file

inputname=ARGV[0]
outputname=ARGV[1]

ARGV.each do |inputname|
  puts(inputname)

  fp = File.open(inputname,'r')

  index=0
  secondsIndex=distanceIndex=gradeIndex=speedIndex=wattsIndex=hrIndex=cadenceIndex=caloriesIndex=tssIndex=0

  line=fp.gets
  line.gsub!("\"", "")
  line.gsub!(",","\t")          # might be tabs or commas
  fields=line.split("\t")
  fields.each do |field|
    secondsIndex =index if field.capitalize=="Timemilliseconds"
    distanceIndex=index if field.capitalize=="Distance"
    gradeIndex   =index if field.capitalize=="Grade"
    speedIndex   =index if field.capitalize=="Speed"
    wattsIndex   =index if field.capitalize=="Watts"
    hrIndex      =index if field.capitalize=="Heartrate"
    cadenceIndex =index if field.capitalize=="Cadence"
    caloriesIndex=index if field.capitalize=="Calories"
    tssIndex     =index if field.capitalize=="Tss"

    index        =index+1
  end

  altitude=0.0
  last_distance=0.0

  loop do                       # read all lines of the file
    break if not line = fp.gets
    line.gsub!('"', '')
    line.gsub!(",","\t")        # might be tabs or commas
    values = line.split("\t")
    
    seconds=(values[secondsIndex].to_f/1000).round
    distance=values[distanceIndex].to_f
    grade=values[gradeIndex].to_f
    speed=values[speedIndex].to_f
    watts=values[wattsIndex].to_i
    hr=values[hrIndex].to_i
    cadence=values[cadenceIndex].to_i
    calories=values[caloriesIndex].to_i
    tss=values[tssIndex].to_i

    altitude = 0 #altitude+grade*(distance-last_distance)

    next if(seconds==0.0 && currentTime!=0.0) # don't print the 0 time line unless first file

    printf(op,"%9.2f,", seconds+currentTime)
    printf(op,"%9.1f,", watts)
    printf(op,"%3d,",hr)
    printf(op,"%4d,",cadence)
    printf(op,"%9.2f,",speed*3600/1000)
    printf(op,"%9.1f,",distance)
    printf(op,"%6.1f,",altitude)
    printf(op,"       0,  0.000\n")
  end
  currentTime=seconds

  fp.close

end
