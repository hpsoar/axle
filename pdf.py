# need to set project to project root
import os
import utils

def get_src_file_path(module):
  return  'src/' + module

def get_inc_file_path(module):
  return 'include/' + utils.project + '/' + module

def get_inc_path(module):
  return get_inc_file_path(module) + '/'

def module_exists(module):
  src_path = get_src_file_path(module)
  inc_path = get_inc_file_path(module)
  if os.path.exists(src_path) and os.path.exists(inc_path):
    return True
  else:
    return False

def module_exist_hint(module):
   hint = 'make sure directory %s exists under both include/%s/ and src/'
   print(hint % (module, utils.project))

def add_to_building_list(module):
  f = open('src/CMakeLists.txt', 'r')
  lines = f.readlines()
  f.close()
  f = open('src/CMakeLists.txt', 'w')
  tag = '#$MODULE$'
  content = 'add_module(%s)\n%s' % (module, tag);
  for line in lines:
    f.write(line.replace(tag, content))
  f.close()

def create_module(module):
  src_path = get_src_file_path(module)
  inc_path = get_inc_file_path(module)
  utils.create_path(src_path)
  utils.create_path(inc_path)
  add_to_building_list(module)

def list_modules():
  utils.list_modules('include/%s' % utils.project)

def get_inc_content(module, guard):
  content = "#ifndef %s\n" % guard
  content += "#define %s\n\n" % guard
  content += "namespace %s {\n" % utils.project
  content += "namespace %s {\n\n" % module
  content += "} // %s\n" % module
  content += "} // %s\n\n" % utils.project
  content += "\n#endif %s\n" % guard
  return content

def get_src_content(module, inc_file):
  content = '#include "%s"\n\n' % inc_file
  content += 'namespace %s {\n' % utils.project
  content += 'namespace %s {\n\n' % module
  content += '} // %s\n' % module
  content += '} // %s\n\n' % utils.project
  return content
