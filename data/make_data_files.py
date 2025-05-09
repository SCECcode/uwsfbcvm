#!/usr/bin/env python

##
#  Builds the data files in the expected format from 
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
  print("\nUWSFBCVM,--- USING urllib.request\n")
  from urllib.request import urlopen
else:
  print("\nUWSFBCVM,--- USING urllib2\n")
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


## at UWSFBCVM/SFB_Vp_Model0.txt

model = "UWSFBCVM"

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
  print("\nUWSFBCVM,--- downloading... \n",url)
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
        continue
    if path == "" :
        print("ERROR: failed to find variables from config file")
        sys.exit(1)

    fp.close()

    print("\nDownloading model file\n")

#    fname="./"+"SFB_Vp_Model0.txt"
    fname="./"+"Vs_model_i0.txt";
    url = path + "/" + fname
    download_urlfile(url,fname)
    fname="./"+"Vp_model_i0.txt";
    url = path + "/" + fname
    download_urlfile(url,fname)

    subprocess.check_call(["mkdir", "-p", mdir])

    # Now we need to go through the data files and put them in the correct
    # format for LSU_IV. More specifically, we need a Vp.dat

    fvs = open("./Vs_model_i0.txt");
    f_vs = open("./uwsfbcvm/vs.dat", "wb")

    f_easting = open("./uwsfbcvm/easting.dat", "wb")
    f_northing = open("./uwsfbcvm/northing.dat", "wb")

    vp_arr = array.array('f', (-1.0,) * (dimension_x * dimension_y * dimension_z))
    vs_arr = array.array('f', (-1.0,) * (dimension_x * dimension_y * dimension_z))
    easting_arr = array.array('f', (-1,) * (dimension_x * dimension_y * dimension_z))
    northing_arr = array.array('f', (-1,) * (dimension_x * dimension_y * dimension_z))

    print ("dimension is", (dimension_x * dimension_y * dimension_z))

    fvp = open("./Vp_model_i0.txt");
    f_vp = open("./uwsfbcvm/vp.dat", "wb")
    vp_nan_cnt = 0
    vp_total_cnt =0;

    x_pos=0;
    y_pos=0;
    z_pos=0;

    for line in fvp:
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
           vp_nan_cnt = vp_nan_cnt + 1

        vp_total_cnt = vp_total_cnt + 1

        loc =z_pos * (dimension_y * dimension_x) + (y_pos * dimension_x) + x_pos
        vp_arr[loc] = vp
        easting_arr[loc] = easting_v
        northing_arr[loc] = northing_v

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

    fvp.close()
    f_vp.close()

    f_easting.close()
    f_northing.close()

    fvs = open("./Vs_model_i0.txt");
    f_vs = open("./uwsfbcvm/vs.dat", "wb")
    vs_nan_cnt = 0
    vs_total_cnt =0;

    x_pos=0;
    y_pos=0;
    z_pos=0;

    for line in fvs:
        arr = line.split()

        vs = -1.0
        tmp = arr[3]

        if( tmp != "NaN" ) :
           vs = float(tmp)
           vs = vs * 1000.0;
        else:
           vs_nan_cnt = vs_nan_cnt + 1

        vs_total_cnt = vs_total_cnt + 1

        loc =z_pos * (dimension_y * dimension_x) + (y_pos * dimension_x) + x_pos
        vs_arr[loc] = vs

        x_pos = x_pos + 1
        if(x_pos == dimension_x) :
          x_pos = 0;
          y_pos = y_pos+1
          if(y_pos == dimension_y) :
            y_pos=0;
            z_pos = z_pos+1
            if(z_pos == dimension_z) :
              print ("All DONE")

    vs_arr.tofile(f_vs)
    fvs.close()
    f_vs.close()

    print("Done! with NaN(", vp_nan_cnt, ") total(", vp_total_cnt,")")
    print("Done! with NaN(", vs_nan_cnt, ") total(", vs_total_cnt,")")


if __name__ == "__main__":
    main()

