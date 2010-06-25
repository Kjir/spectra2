#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import struct
import numpy
from optparse import OptionParser
import pylab

op = OptionParser()
op.add_option("-f", dest="filename", default="file.sig")
op.add_option("-l", dest="length", type="int", default=256)
eps = 1
opts, args = op.parse_args(sys.argv[1:])
fmt = "%df" % opts.length
fin = open(opts.filename, "rb")
nchan = opts.length
buffer = fin.read(4 * opts.length)
data = struct.unpack(fmt, buffer)
data = numpy.array(data[:nchan//2] + data[nchan//2+1:], dtype="float")
for i,d in enumerate(data):
    if d == 0:
        data[i] = eps
data = numpy.multiply(10, numpy.log10(data))
pylab.plot(data)
pylab.show()
