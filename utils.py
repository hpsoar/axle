import os

project = ''
cmd_name = ''

def add_svn(path):
  os.system('svn add %s' % path)

def list_modules(module_path):
  for dirname, dirnames, filenames in os.walk(module_path):
    for subdir in dirnames:
      print(subdir)

def create_path(path):
  if not os.path.exists(path):
    os.makedirs(path)
    add_svn(path)

def get_inc_filename(inc_path, src):
  ext = os.path.splitext(src)[1]
  inc_file = inc_path
  if ext:
    inc_file += src.replace(ext, '.h')
  else:
    inc_file += src + '.h'
  return inc_file

def get_guard_macro(module, inc):
  guard = project + '_' + module + '_' + inc.replace('.', '_')
  return guard.upper()
