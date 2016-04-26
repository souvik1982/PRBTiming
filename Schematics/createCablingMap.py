# === Create Cabling Map for PRB Timing Software ===
# Author: Souvik Das
# Date: Apr 26, 2016
# ==================================================

import datetime
today=datetime.date.today()

import urllib2
prb=0
inFile = urllib2.urlopen("https://raw.githubusercontent.com/zhenhu/TrackTriggerDemo/master/inputfiles/ModuleMap01.txt")
outFile = open("Schematic_CablingMap_PRB"+str(prb)+".txt", 'w')

outFile.write("# === Cabling Map from CICs to PRB"+str(prb)+" === \n")
outFile.write("# Author: Souvik Das \n")
outFile.write("# Date: "+today.strftime("%b %d, %Y")+"\n")
outFile.write("# Built using createCablingMap.py \n")
outFile.write("# and existing CIC --> PRB cabling maps \n")
outFile.write("# ===================================== \n \n")

index=0
for line in inFile:

  outFile.write("Index: "+str(prb*1000+index*2)+"\n")
  outFile.write("ComponentType: CIC \n")
  outFile.write("ComponentName: cicL"+str(prb*1000+index*2)+"\n")
  outFile.write("ModuleID: "+line)
  outFile.write("Segment: L \n")
  outFile.write("Frequency: 250 MHz \n")
  outFile.write("OutputConnections: ("+str(10000+prb*10)+", 0, "+str(index)+") \n")
  outFile.write("\n")
  
  outFile.write("Index: "+str(prb*1000+index*2+1)+"\n")
  outFile.write("ComponentType: CIC \n")
  outFile.write("ComponentName: cicR"+str(prb*1000+index*2+1)+"\n")
  outFile.write("ModuleID: "+line)
  outFile.write("Segment: R \n")
  outFile.write("Frequency: 250 MHz \n")
  outFile.write("OutputConnections: ("+str(10000+prb*10+1)+", 0, "+str(index)+") \n")
  outFile.write("\n")
  
  index+=1
