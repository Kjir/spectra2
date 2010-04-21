#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import socket
from optparse import OptionParser

op = OptionParser()
op.add_option("-H", dest="host", default="localhost")
op.add_option("-p", dest="port", type="int", default=59045)
op.add_option("-f", dest="file", default="-")
op.add_option("-l", dest="dgram_length", type="int", default=512)
opts, args = op.parse_args(sys.argv[1:])
file = None
if opts.file == "-":
    file = sys.stdin
else:
    file = open(opts.file, "rb")

counter = 0

udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

try:
    while True:
        buffer = file.read(opts.dgram_length)
        if buffer == '':
            raise Exception("EOF")
        str_count = struct.pack("L", counter)
        sent = udp.sendto(str_count + buffer, (opts.host, opts.port))
        print >> sys.stderr, "Sent %d bytes" % sent
        counter += 1
except Exception:
    pass
finally:
    udp.close()
