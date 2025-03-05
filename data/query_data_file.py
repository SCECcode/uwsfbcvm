#!/usr/bin/env python

##
# query the utm mesh with latlon
#

#-116.051578 32.686249 0
#-116.051578 32.686249 1000
#-116.051578 32.686249 2000
#-116.0516    32.6862      0.000    475.268    533.575       none      0.000      0.000      0.000
#       none      0.000      0.000      0.000       none      0.000      0.000      0.000
#-116.0516    32.6862   1000.000    475.268    533.575      ivlsu   4093.000   2357.458   2406.281
#       none      0.000      0.000      0.000      crust   4093.000   2357.458   2406.281
#-116.0516    32.6862   2000.000    475.268    533.575      ivlsu   4838.500   2906.943   2510.425
#       none      0.000      0.000      0.000      crust   4838.500   2906.943   2510.425


import getopt
import sys
import subprocess
import struct
import numpy as np
import utm
import pdb

dimension_x = 66
dimension_y = 86 
dimension_z = 9

lon_origin = -116.051578
lat_origin = 32.596922
lon_upper = -115.344866
lat_upper = 33.356203

easting_origin = 589000
northing_origin = 3607000
easting_upper = 654000
northing_upper = 3692000

delta_easting = (easting_upper - easting_origin )/(dimension_x-1)
delta_northing = (northing_upper - northing_origin)/(dimension_y-1)

rdelta_lon = (lon_upper - lon_origin )/(dimension_x-1)
rdelta_lat = (lat_upper - lat_origin)/(dimension_y-1)
delta_lon = float('%1.2f'%rdelta_lon)
delta_lat = float('%1.2f'%rdelta_lat)

#target_lat = -115.88
#target_lon = 32.61
#target_depth = 0
target_lat = -116.051578
target_lon = 32.686249
target_depth = 1


def main():

    zone=11
    f_vp = open("./ivlsu/vp.dat")
    vp_arr = np.fromfile(f_vp, dtype=np.float32)
    f_vp.close()

    (reasting_v, rnorthing_v, zone, zone_letter) = utm.from_latlon(target_lon, target_lat)
    easting_v=round(reasting_v /1000)* 1000
    northing_v=round(rnorthing_v /1000)* 1000

    if (easting_v < easting_origin) | (easting_v > easting_upper) :
       print "OUT of bound -- X/easting"
       exit(1)
    if (northing_v < northing_origin) | (northing_v > northing_upper) :
       print "OUT of bound -- Y/northing"
       exit(1)

    x_pos = int(round((easting_v - easting_origin) / delta_easting))
    y_pos = int(round((northing_v - northing_origin) / delta_northing))
    z_pos = int(target_depth)

    offset=z_pos * (dimension_y * dimension_x) + (y_pos * dimension_x) + x_pos

    vp=vp_arr[offset];

    print "offset:", offset;
    print "xyz:", x_pos,y_pos,z_pos,"(",target_lat,target_lon,")>>", easting_v,northing_v,"-->vp", vp

    print "Done!"

if __name__ == "__main__":
    main()


