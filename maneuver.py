#!/usr/bin/env python3

import os

        #set key fixed right top vertical autotitle
        #set key fixed right top vertical autotitle
        #set key fixed right top vertical autotitle columnheader;
scriptold = """ \
        data = "maneuver.csv";
        set datafile columnheader;
        set key opaque autotitle columnhead;
        plot for [i=2:3] data using 1:i title columnhead(i) with lines;
        exit;
        set datafile columnheaders;
        set key autotitle columnhead ;
        plot data using 1:2 with lines, data using 1:3 with lines;
        exit;
        set key autotitle columnhead opaque;
        plot "maneuver.csv" with lines, "maneuver.csv" using 1:3 with lines;
        exit;
        set datafile columnheaders;
        set key autotitle columnhead opaque;
        plot "maneuver.csv" using 1:2 with lines title columnheader(1), "maneuver.csv" using 1:3 with lines title columnheader(3);
        exit;
        set key autotitle columnheader;
        plot "maneuver.csv" using 1:3 with lines title columnhead(2), "maneuver.csv" using 1:2 with lines title columnhead(3);
        exit;
        plot "maneuver.csv" using 1:3 with lines title columnhead(3), "maneuver.csv" using 1:2 with lines title columnhead(2)
"""


        #plot "+" using ($1):(sin($1)):(sin($1)**2) with filledcurves;
        #set grid
        #set xtics

old2script = """ \
        set term wxt background rgb "0x333333" size 400,700 position 15,25;
        set border lc "0x00f0f0f0";
        set key tc rgb "0x00f0f0f0" box lc "0x00f0f0f0";
        data = "maneuver.csv";
        set datafile columnheader;
        set datafile separator ",";
        set key opaque autotitle columnhead;
        plot data using 1:3 title columnhead(3) with lines lc rgbcolor "0x00c9211a", data using ($1):(2.23694*($2)) title "".columnhead(2)." (mph)" with lines lc rgbcolor "0x001f77b4";
        exit;
        plot for [i=2:3] data using 1:i title columnhead(i) with lines, data using ($1):(2.23694*($2)) title "".columnhead(2)." (mph)" with lines lc rgbcolor "0x001f77b4";
"""
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

# vego mph #1f77b4
# aego #c9211a
# bg #333333
# fg #444444
# white f0f0f0

os.system("gnuplot -p -e '" + script + "'")

exit()

with open("maneuver.csv", "r") as f:
  import csv
  reader = csv.DictReader(f)
  for row in reader:
    f = float(row['t'])
    print(f, end='')
    f = float(row['v'])
    print(f, end='')
    f = float(row['a'])
    print(f)
