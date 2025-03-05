#!/usr/bin/env python

##
#  Retrieve the data files and retrieve the content
#  ./make_data_file.py > ii
#  ./test_data_file.py > oo
#  cat ii oo | sort | uniq -c |sort > zz
#  should be 2x of every valid entries in the model
#

import getopt
import sys
import subprocess
import struct
import numpy as np
import pdb

dimension_x = 66
dimension_y = 86
dimension_z = 9

lon_origin = -116.051578
lat_origin = 32.596922
lon_upper = -115.344866
lat_upper = 33.356203

easting_origin = 589
northing_origin = 3607
easting_upper = 654
northing_upper = 3692


delta_lon = (lon_upper - lon_origin )/(dimension_x-1)
delta_lat = (lat_upper - lat_origin)/(dimension_y-1)

def usage():
    print("\n./test_data_files.py\n\n")
    sys.exit(0)

def myrange(start, end, step):
    while start < end+(step/2):
        yield start
        start = start + step

def main():

    f_vp = open("./ivlsu/vp.dat")
    vp_arr = np.fromfile(f_vp, dtype=np.float32)
    f_vp.close()

    lon_start = lon_origin
    lat_start = lat_origin
    nan_cnt=0
    total_cnt=0

    for lon_v in myrange(lon_origin, lon_upper, delta_lon):
        for lat_v in myrange(lat_origin, lat_upper, delta_lat) :
            for depth_v in xrange(9) :
               y_pos = int(round((lat_v - lat_origin) / delta_lat))
               x_pos = int(round((lon_v - lon_origin) / delta_lon))
               z_pos = int(depth_v)

               offset=z_pos * (dimension_y * dimension_x) + (y_pos * dimension_x) + x_pos
               vp=vp_arr[offset];

               total_cnt=total_cnt+1
               if vp != -1 :
                 print offset,":",x_pos," ",y_pos," ",z_pos," >> ",lon_v," ",lat_v," ",float(depth_v)," ",vp
               else :
                 nan_cnt=nan_cnt+1
                 print "NAN", x_pos," ",y_pos," ",z_pos," >> ",lon_v," ",lat_v," ",float(depth_v)," ",vp

    print("Done! with NaN",nan_cnt,"total",total_cnt)

if __name__ == "__main__":
    main()


