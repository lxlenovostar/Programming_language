#! /bin/python 

import pickle 

pickle_file = open('some_dict.pkl', 'r')
another_name_for_some_dict = pickle.load(pickle_file)
print another_name_for_some_dict
