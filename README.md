# UW San Francisco Bay Community Velocity Model

[![License](https://img.shields.io/badge/License-BSD_3--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)
![GitHub repo size](https://img.shields.io/github/repo-size/sceccode/uwsfbcvm)
[![uwsfbcvm-ucvm-ci Actions Status](https://github.com/SCECcode/uwsfbcvm/workflows/uwsfbcvm-ucvm-ci/badge.svg)](https://github.com/SCECcode/uwsfbcvm/actions)

The uwsfbcvm velocity model describes 3D seismic P- and S-wave velocities from a
tomographic inversion for crustal structure in the San Francisco Bay region. The
inversion used a modified version of the code by Fang et al. (JGR, 2016), which
inverts both body-wave arrival times and surface-wave dispersion measurements 
for 3D P- and S-wave velocity structure simultaneously with determining earthquake
locations.

## Installation

This package is intended to be installed as part of the UCVM framework
version 25.x or higher.

## Library

The library ./lib/libsfbcvm.a may be statically linked into any
user application. Also, if your system supports dynamic linking,
you will also have a ./lib/libsfbcvm.so file that can be used
for dynamic linking. The header file defining the API is located
in ./include/sfbcvm.h.

## Contact the authors

If you would like to contact the authors regarding this software,
please e-mail software@scec.org. Note this e-mail address should
be used for questions regarding the software itself (e.g. how
do I link the library properly?). Questions regarding the model's
science (e.g. on what paper is the UWSFBCVM based?) should be directed
to the model's authors, located in the AUTHORS file.

## Note

A right rectangle, no rotation 

Density is calculated, from https://pubs.usgs.gov/of/2005/1317/of2005-1317.pdf

<pre>

  *[eqn. 6] r (g/cm3) = 1.6612Vp – 0.4721Vp2 + 0.0671Vp3 – 0.0043Vp4 + 0.000106Vp5

</pre>


