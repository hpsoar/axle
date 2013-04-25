import os
import re

inc_dir = 'include/'
src_dir = 'src/'

ns1 = 'namespace\s*cheetah\s*{'
re_ns1 = re.compile(ns1)
ns2 = 'namespace\s*[a-z]*\s*{'
re_ns2 = re.compile(ns2)
ns3 = '}\s*//[a-z]*\s*\n'
#ns_e = re.compile(ns2 * 2 + '[\s\S]*\#endif\s*[A-Z,_]*')

def reformfile(filename):
  lines = open(filename).readlines()
  #f = open(filename, 'w')
  for line in lines:
    if re_ns1.match(line):
      print(line)
    if re_ns2.match(line):
      print(line)
    #f.write(line)


def reform(path):
  for (dirpath, dirnames, filenames) in os.walk(path):
    for filename in filenames: reformfile(dirpath + os.sep + filename)
    for dirname in dirnames: reform(dirpath + os.sep + path) 


reform(inc_dir)
reform(src_dir)

