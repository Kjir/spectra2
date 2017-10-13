#!/usr/bin/env python

import numpy
import pylab
import wave

a = wave.open('segnale.wav')
snd = numpy.fromstring(a.readframes(a.getnframes()), numpy.int16)
time = pylab.arange(0, float(a.getnframes()), 1)
time = time / a.getframerate()
time = time * 1000 # Convert to ms
pylab.plot(time, snd, color='r')
pylab.ylabel('Pressione')
pylab.xlabel('Tempo (ms)')
print "Writing plot to image..."
pylab.savefig('segnale.svg')
pylab.savefig('segnale.pdf')
pylab.clf()
print "... Done!"

n = len(snd)
freq = pylab.fft(snd)

n_uniq = pylab.ceil((n+1)/2.0)
freq = freq[0:n_uniq] # Eliminate repeated information
freq = abs(freq) # Get magnitude only
freq = freq / float(n)
freq = freq**2
if n % 2 > 0: # we've got odd number of points fft
    freq[1:len(freq)] = freq[1:len(freq)] * 2
else:
    freq[1:len(freq) -1] = freq[1:len(freq) - 1] * 2 # we've got even number of points fft
pylab.plot(10*pylab.log10(freq), color='r')
pylab.xlabel('Frequenza (Hz)')
pylab.ylabel('Potenza (dB)')
pylab.savefig('frequenza.svg')
pylab.savefig('frequenza.pdf')
