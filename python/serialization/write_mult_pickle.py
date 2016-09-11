#! /bin/python 

import pickle 

list_of_dicts = [{str(i):i} for i in range(5)]
pickle_file = open('list_of_dicts.pkl', 'w')

for d in list_of_dicts: 
    pickle.dump(d, pickle_file)

pickle_file.close()
