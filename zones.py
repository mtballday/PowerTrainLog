#!/usr/bin/python3

import sys

threshold=int(sys.argv[1])
print("FTP: {}".format(threshold))
print("Zone    \tPercent\tUpper\tRounded")
print("Recovery\t55%\t{}\t{}".format(round(threshold*0.55),round(threshold*0.55,-1)))
print("L2 Endurance\t75% \t{}\t{}"    .format(round(threshold*0.75),round(threshold*0.75,-1)))
print("L3 Tempo\t90% \t{}\t{}"    .format(round(threshold*0.90),round(threshold*0.90,-1)))
print("L4 Threshold\t105% \t{}\t{}".format(round(threshold*1.05),round(threshold*1.05,-1)))
print("L5 VO2 Max\t120% \t{}\t{}".format(round(threshold*1.2),round(threshold*1.2,-1)))
print("L6 Anaerobic\t150% \t{}\t{}".format(round(threshold*1.5),round(threshold*1.5,-1)))
print("L7 NP      \t>    \t{}".format(1000))

print("Sweet Spot: \t{} \t{}".format(round(threshold*.88),round(threshold*.92)))
