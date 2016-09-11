#! /bin/python 

import pickle 

pickle_file = open('list_of_dicts.pkl', 'r')

while 1:
    try:
        print pickle.load(pickle_file)
    except EOFError:
        print "EOF Error"
        break 


