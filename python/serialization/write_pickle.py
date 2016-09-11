#! /bin/python 

import pickle 

some_dict = {'a':1, 'b':2}
pickle_file = open('some_dict.pkl', 'w')
pickle.dump(some_dict, pickle_file)
pickle_file.close()
