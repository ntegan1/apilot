#!/usr/bin/env python3

import os
import sys
import cereal.messaging as messaging

csv_file = sys.argv[1]

script = """ \
        set mxtics;
        set mytics;
        set term wxt background rgb "0x333333" size 400,700 position 15,25;
        set border lc "0x00f0f0f0";
        set key tc rgb "0x00f0f0f0" box lc "0x00f0f0f0";
        data = "maneuver.csv";
        set datafile columnheader;
        set datafile separator ",";
        set key opaque autotitle columnhead;
        set grid xtics mxtics lc rgb "0x00444444", lc rgb "0x40444444";
        set grid ytics mytics lc rgb "0x00f0f0f0", lc rgb "0x40f0f0f0";
        plot data using 1:3 title columnhead(3) with lines lc rgbcolor "0x00c9211a", data using ($1):(2.23694*($2)) title "".columnhead(2)." (mph)" with lines lc rgbcolor "0x001f77b4";
"""

pm = messaging.PubMaster(['testJoystick'])


def get_tva():
  with open(csv_file, "r") as f:
    import csv
    reader = csv.DictReader(f)
    t = []
    v = []
    a = []
    for row in reader:
      t.append(float(row["t"]))
      v.append(float(row["v"]))
      a.append(float(row["a"]))
    return t, v, a

tva = get_tva()
#os.system("gnuplot -p -e '" + script + "'")

dat = messaging.new_message('testJoystick')
maneuver = dat.testJoystick.maneuver
mp = maneuver.init("maneuverPlan")
plan = mp.init("plan", len(tva[0]))
for i in range(len(tva[0])):
  plan[i].t = tva[0][i]
  plan[i].v = tva[1][i]
  plan[i].a = tva[2][i]
print(plan)
pm.send('testJoystick', dat)

import time
time.sleep(1.5)

dat = messaging.new_message('testJoystick')
maneuver = dat.testJoystick.maneuver
maneuver.maneuverBegin = None
pm.send('testJoystick', dat)

exit()

