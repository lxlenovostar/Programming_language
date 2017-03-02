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
    return dict([(w,1) for w in words])

def sampletrain(cl):
    cl.train('Nobody owns the water.','good')
    cl.train('the quick rabbit jumps fences','good')
    cl.train('buy pharmaceuticals now','bad')
    cl.train('make quick money at the online casino','bad')
    cl.train('the quick brown fox jumps','good')

class classifier:
    def __init__(self, getfeatures, filename=None):
        # Counts of feature/category combinations
        #
        # The fc variable will store the counts for different features in different 
        # classifications. For example:
        # {'python': {'bad': 0, 'good': 6}, 'the': {'bad': 3, 'good': 3}}
        # This indicates that the word “the” has appeared in documents classified as bad three
        # times, and in documents that were classified as good three times. The word “Python”
        # has only appeared in good documents.
        self.fc = {}
                
        # Counts of documents in each category
        # The cc variable is a dictionary of how many times every classification has been used.
        self.cc = {}

        # getfeatures, is the function that will be used to extract the features from the items 
        # being classified—in this example, it is the getwords function you just defined.
        self.getfeatures = getfeatures

    # Increase the count of a feature/category pair
    def incf(self, f, cat):
        self.fc.setdefault(f,{})
        self.fc[f].setdefault(cat,0)
        self.fc[f][cat]+=1

    # Increase the count of a category
    def incc(self,cat):
        self.cc.setdefault(cat,0)
        self.cc[cat] += 1

    # The number of times a feature has appeared in a category
    def fcount(self, f, cat):
        if f in self.fc and cat in self.fc[f]:
            return float(self.fc[f][cat])
        return 0.0

    # The number of items in a category
    def catcount(self,cat):
        if cat in self.cc:
            return float(self.cc[cat])
        return 0

    # The total number of items
    def totalcount(self):
        return sum(self.cc.values( ))

    # The list of all categories
    def categories(self):
        return self.cc.keys( )

    def train(self,item,cat):
        features=self.getfeatures(item)
        # Increment the count for every feature with this category
        for f in features:
            self.incf(f,cat)

        # Increment the count for this category
        self.incc(cat)

    def fprob(self, f, cat):
        if self.catcount(cat)==0: return 0

        # The total number of times this feature appeared in this
        # category divided by the total number of items in this category
        return self.fcount(f, cat) / self.catcount(cat)

    # a weight of 1 means the assumed probability is weighted the
    # same as one word. 
    def weightedprob(self, f, cat, prf, weight=1.0, ap=0.5):
        # Calculate current probability
        basicprob = prf(f,cat)
        # Count the number of times this feature has appeared in
        # all categories
        totals=sum([self.fcount(f,c) for c in self.categories()])
        # Calculate the weighted average
        bp = ((weight*ap) + (totals*basicprob)) / (weight+totals)
        return bp

class naivebayes(classifier):
    def __init__(self, getfeatures):
        classifier.__init__(self,getfeatures)
        self.thresholds={}

    def docprob(self,item,cat):
        features=self.getfeatures(item)

        # Multiply the probabilities of all the features together
        p=1
        for f in features: p*=self.weightedprob(f,cat,self.fprob)
        return p

    # 根据贝叶斯公式
    def prob(self,item,cat):
        catprob = self.catcount(cat)/self.totalcount()
        docprob = self.docprob(item,cat)

        return docprob*catprob

    def setthreshold(self,cat,t):
        self.thresholds[cat]=t

    def getthreshold(self,cat):
        if cat not in self.thresholds: return 1.0
        return self.thresholds[cat]

    def classify(self, item, default=None):
        probs={}

        # Find the category with the highest probability
        max=0.0

        for cat in self.categories():
            probs[cat]=self.prob(item,cat)
            if probs[cat] > max:
                max = probs[cat]
                best = cat

        # Make sure the probability exceeds threshold*next best
        for cat in probs:
            if cat == best: 
                continue

            if probs[cat]*self.getthreshold(best) > probs[best]: 
                return default
        
        return best
