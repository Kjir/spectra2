#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import struct
import numpy
from optparse import OptionParser
import os
import pylab

def get_int_time(bw, nch, nint):
    return (2.0*nch*nint) / (bw*1e6*2.0) 

def get_params(filename):
    params = [int(i) for i in filename[:-4].split("_")]
    t = get_int_time(*params)
    return {'bw' : params[0],
            'nchan' : params[1],
            'nint' : params[2],
            'time' : t,
            }

def print_image(infilename, indir, outdir):
    eps = 1.0
    outfilename = os.path.join(outdir, infilename[:-4] + ".png")
    params = get_params(infilename)
    #read data from input file 
    fin = open(os.path.join(indir, infilename), "rb")
    fmt = "%df" % params['nchan']
    buffer = fin.read(4 * params['nchan'])
    fin.close()
    data = struct.unpack(fmt, buffer)
    data = numpy.array(data[:params['nchan']//2] + data[params['nchan']//2+1:], dtype="float")
    for i,d in enumerate(data):
	if d == 0:
            data[i] = eps
    data = numpy.multiply(10, numpy.log10(data))
    pylab.title(str(params['bw']) + "MHz bandwidth, " + 
                str(params['nchan']) + " channels, " + 
                str(params['nint']) + " integrations = " +
                str(params['time']) + " sec.")
    pylab.grid()
    pylab.plot(data)
    pylab.xlabel("Frequency channels, res = " + str(float(params['bw']*1e6) / float(params['nchan'])) + "Hz/chan")
    pylab.ylabel("Power (db scale)")
    pylab.savefig(outfilename)
    pylab.close()
    
op = OptionParser()
op.add_option("-d", dest="dirname", default="tests", help="directory name where to find spectrums files")
eps = 1
opts, args = op.parse_args(sys.argv[1:])
dirname = opts.dirname
#create image output directory
image_dir = os.path.join(dirname, "images")
if not os.path.exists(image_dir):
    os.makedirs(image_dir)
#get input filenames
filenames = [i for i in os.listdir(dirname) if i.endswith(".dat")]
for f in filenames:
    print_image(f, dirname, image_dir)
