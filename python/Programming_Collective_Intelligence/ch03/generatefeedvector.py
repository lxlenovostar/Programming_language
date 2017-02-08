#!/usr/bin/python
# -*- coding: utf-8 -*-  

import feedparser
import re

def getwords(html):
	# Remove all the HTML tags
    # ^代表非的意思，而+代表至少出现１次，[]代表正则表达中的字符组
	txt = re.compile(r'<[^>]+>').sub('', html)

	# Split words by all non-alpha characters
	words = re.compile(r'[^A-Z^a-z]+').split(txt)

	# Convert to lowercase
	return [word.lower( ) for word in words if word!='']

# Returns title and dictionary of word counts for an RSS feed
def getwordcounts(url):
	# Parse the feed
	d = feedparser.parse(url)
	wc = {}

	# Loop over all the entries
	for e in d.entries:
		if 'summary' in e: 
			summary = e.summary
		else: 
			summary = e.description

		# Extract a list of words
		words = getwords(e.title + ' ' + summary)
		for word in words:
			wc.setdefault(word, 0)
			wc[word] += 1

	return d.feed.title, wc
