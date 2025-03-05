#!/usr/bin/env python

##
#  Builds the data files in the expected format from IV33.dat.txt
#
# from >> easting(Km) northing(Km) depth(Km) vp(km/s)
#
# to >>  /** P-wave velocity in km/s per second */
#        double vp;
# depth is in increment of 1000m,
#
#The columns of the file are: x (km) y (km) z (km) vp (km/s), where 
#x, y, and z are utm coordinates in km and the columns increase in x.y.z order. 
#Grid spacing is 1 km in each spatial dimension. 

import getopt
import sys
import subprocess
import struct
import array

if sys.version_info.major >= (3) :
  from urllib.request import urlopen
else:
  from urllib2 import urlopen

##import osr
##def transform_utm_to_wgs84(easting, northing, zone):
##    utm_coordinate_system = osr.SpatialReference()
##          # Set geographic coordinate system to handle lat/lon
##    utm_coordinate_system.SetWellKnownGeogCS("WGS84") 
##
##    is_northern = northing > 0    
##    utm_coordinate_system.SetUTM(zone, is_northern)
##          # Clone ONLY the geographic coordinate system 
##    wgs84_coordinate_system = utm_coordinate_system.CloneGeogCS() 
##          # create transform component
##          # (<from>, <to>)
##    utm_to_wgs84_transform = osr.CoordinateTransformation(utm_coordinate_system, wgs84_coordinate_system) 
##          # returns lon, lat, altitude
##    return utm_to_wgs84_transform.TransformPoint(easting, northing, 0) 


## at SFBCVM/SFB_Vp_Model0.txt

model = "SFBCVM"

dimension_x = 0
dimension_y = 0 
dimension_z = 0

lon_origin = 0
lat_origin = 0

lon_upper = 0
lat_upper = 0

def usage():
    print("\n./make_data_files.py\n\n")
    sys.exit(0)

def download_urlfile(url,fname):
  try:
    response = urlopen(url)
    CHUNK = 16 * 1024
    with open(fname, 'wb') as f:
      while True:
        chunk = response.read(CHUNK)
        if not chunk:
          break
        f.write(chunk)
  except:
    e = sys.exc_info()[0]
    print("Exception retrieving and saving model datafiles:",e)
    raise
  return True

def main():

    # Set our variable defaults.
    path = ""
    mdir = ""

    try:
        fp = open('./config','r')
    except:
        print("ERROR: failed to open config file")
        sys.exit(1)

    ## look for model_data_path and other varaibles
    lines = fp.readlines()
    for line in lines :
        if line[0] == '#' :
          continue
        parts = line.split('=')
        if len(parts) < 2 :
          continue;
        variable=parts[0].strip()
        val=parts[1].strip()

        if (variable == 'model_data_path') :
            path = val + '/' + model
            continue
        if (variable == 'model_dir') :
            mdir = "./"+val
            continue
        if (variable == 'nx') :
            dimension_x = int(val)
            continue
        if (variable == 'ny') :
            dimension_y = int(val)
            continue
        if (variable == 'nz') :
            dimension_z = int(val)
            continue
        if (variable == 'bottom_left_corner_lon') :
            lon_origin = float(val)
            continue
        if (variable == 'bottom_left_corner_lat') :
            lat_origin = float(val)
            continue
        if (variable == 'top_right_corner_lon') :
            lon_upper = float(val)
            continue
        if (variable == 'top_right_corner_lat') :
            lat_upper = float(val)
            continue

        continue
    if path == "" :
        print("ERROR: failed to find variables from config file")
        sys.exit(1)

    fp.close()

    delta_lon = (lon_upper - lon_origin )/(dimension_x-1)
    delta_lat = (lat_upper - lat_origin)/(dimension_y-1)

    print("\nDownloading model file\n")

    fname="./"+"SFB_Vp_Model0.txt"
    url = path + "/" + fname
#
#    download_urlfile(url,fname)

    subprocess.check_call(["mkdir", "-p", mdir])

    # Now we need to go through the data files and put them in the correct
    # format for LSU_IV. More specifically, we need a Vp.dat

    f = open("./SFB_Vp_Model0.txt")

    f_vp = open("./sfbcvm/vp.dat", "wb")
    f_easting = open("./sfbcvm/easting.dat", "wb")
    f_northing = open("./sfbcvm/northing.dat", "wb")

    vp_arr = array.array('f', (-1.0,) * (dimension_x * dimension_y * dimension_z))
    easting_arr = array.array('f', (-1,) * (dimension_x * dimension_y * dimension_z))
    northing_arr = array.array('f', (-1,) * (dimension_x * dimension_y * dimension_z))

    print ("dimension is", (dimension_x * dimension_y * dimension_z))

    nan_cnt = 0
    total_cnt =0;
    x_pos=0;
    y_pos=0;
    z_pos=0;
    for line in f:
        arr = line.split()

        vp = -1.0
        easting_v = float(arr[0])
        northing_v = float(arr[1])
        depth_v = float(arr[2])
        tmp = arr[3]

        if( tmp != "NaN" ) :
           vp = float(tmp)
           vp = vp * 1000.0;
        else:
           nan_cnt = nan_cnt + 1

        total_cnt = total_cnt + 1

        loc =z_pos * (dimension_y * dimension_x) + (y_pos * dimension_x) + x_pos
        vp_arr[loc] = vp
        easting_arr[loc] = easting_v
        northing_arr[loc] = northing_v

#       print (total_cnt, "loc",loc," ", x_pos," ",y_pos," ",z_pos," >> ",easting_v," ",northing_v," ",depth_v,":",vp )

      
        x_pos = x_pos + 1
        if(x_pos == dimension_x) :
          x_pos = 0;
          y_pos = y_pos+1
          if(y_pos == dimension_y) :
            y_pos=0;
            z_pos = z_pos+1
            if(z_pos == dimension_z) :
              print ("All DONE")

    vp_arr.tofile(f_vp)
    easting_arr.tofile(f_easting)
    northing_arr.tofile(f_northing)

    f.close()
    f_vp.close()
    f_easting.close()
    f_northing.close()

    print("Done! with NaN", nan_cnt, "toal", total_cnt)

if __name__ == "__main__":
    main()

