#!/usr/bin/python

import timeit

t = timeit.Timer(
"""
import hashlib
for line in url_paths:
    h = hashlib.md5(line).hexdigest()
#    h = hashlib.sha1(line).hexdigest()
#    h = hashlib.sha256(line).hexdigest()
#    h = hashlib.sha512(line).hexdigest()
"""
,
"""
url_paths = []
f = open('urls.txt', 'r')
for l in f.readlines():
    url_paths.append(l)
f.close()
"""
)

print t.repeat(repeat=1, number=10)
