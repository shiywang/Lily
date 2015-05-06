#!/usr/bin/python
import re
import os
files = os.listdir('.')
for filename in files:
    print filename
  
    portion = os.path.splitext(filename)
    if portion[1] == '.c' or portion[1] == 'h':
        f1 = open(filename)
        f2 = open(portion[0]+'.temp','w')
        for s in f1.readlines():
            f2.write(s.replace('\t', '    '))
        f1.close()
        f2.close()
        os.remove(filename)
        print 'remove file:' + filename +'\n'   
        os.rename(portion[0]+'.temp',portion[0]+'.c') 
