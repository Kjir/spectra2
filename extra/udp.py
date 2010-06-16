#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import socket
import struct
from optparse import OptionParser

op = OptionParser()
op.add_option("-H", dest="host", default="localhost")
op.add_option("-p", dest="port", type="int", default=59045)
op.add_option("-f", dest="file", default="-")
op.add_option("-l", dest="dgram_length", type="int", default=512)
op.add_option("-s", dest="start_seq", type="int", default=1)

opts, args = op.parse_args(sys.argv[1:])
file = None
if opts.file == "-":
    file = sys.stdin
else:
    file = open(opts.file, "rb")

counter = opts.start_seq

udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

try:
    while True:
        buffer = file.read(opts.dgram_length)
        if buffer == '':
            raise Exception("EOF")
        # This is BigEndian
        #str_count = struct.pack(">Q", counter)
        # This is LittleEndian
        str_count = struct.pack("<Q", counter)
        buffer = str_count + buffer
        sent = udp.sendto(buffer, (opts.host, opts.port))
        print >> sys.stderr, "Sent %d bytes" % sent
        counter += 1
except Exception:
    pass
finally:
    udp.close()
