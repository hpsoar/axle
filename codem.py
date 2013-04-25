import os
import utils
import pdf # project dependent functions

def add_module_cmd():
  return '%s <module>' % utils.cmd_name

def add_file_cmd():
  return '%s <module> <filename[extension]>' % utils.cmd_name

def print_help():
  print('Usage: %s' % add_file_cmd())
  print('Usage: %s' % add_module_cmd())

def file_exists(filename):
  if os.path.exists(filename):
    print('file %s already exists' % filename)
    return True
  else:
    return False

def add_file(filename, content):
  f = open(filename, 'w')
  f.write(content)
  f.close()
  utils.add_svn(filename)

def add_inc(module, filename):
  filepath = pdf.get_inc_file_path(module) + os.sep + filename
  if not file_exists(filepath):
    content = pdf.get_inc_content(
        module, utils.get_guard_macro(module, filename))
    add_file(filepath, content)

def add_src(module, filename): 
  filepath = pdf.get_src_file_path(module) + os.sep + filename
  if not file_exists(filepath):
    content = pdf.get_src_content(
        module, utils.get_inc_filename(pdf.get_inc_path(module), filename))
    add_file(filepath, content)

#make sure name doesn't has a extension
def add_class(module, name): 
  add_inc(module, name + '.h')
  add_src(module, name + '.cc')

def add_code(module, name):
  if pdf.module_exists(module):
    filename, ext = os.path.splitext(name)
    if ext in { '.h', '.hpp' } :
      add_inc(module, name)
    elif ext in { '.c', '.cc', '.cpp', '.cxx' }:
      add_src(module, name)
    elif ext == '':
      add_class(module, name)
    else:
      print('unsupported file extension')
  else:
    print('module %s does not exist or not complete' % module)
    pdf.module_exist_hint(module)
    print('you can add module by: %s' % add_module_cmd())

def add_module(module):
  if pdf.module_exists(module):
    print('module "%s" already exists' % module)
  else:
    pdf.create_module(module) # project dependent

def main_func():
  import sys
  argc = len(sys.argv)
  if argc < 2:
    print_help()
  elif argc < 3:
    if sys.argv[1] == '-l':
      pdf.list_modules()
    else:
      add_module(sys.argv[1])
  elif argc < 4:
    add_code(sys.argv[1], sys.argv[2])
  else:
    print('too many arguments')
