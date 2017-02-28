#!/usr/bin/python
# -*- coding: utf-8 -*-  

import re
import math

# This function breaks up the text into words by dividing the text on any character
# that isn’t a letter. This leaves only actual words, all converted to lowercase.
def getwords(doc):
	splitter=re.compile('\\W*')

	# Split the words by non-alpha characters
	words=[s.lower() for s in splitter.split(doc) if len(s)>2 and len(s)<20]

	# Return the unique set of words only
	return dict([(w,1) for w in words]
