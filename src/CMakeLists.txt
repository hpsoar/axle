set(inc_path ../include/axle)
set(target_name axle)

macro(copy_file SRC_PATH DEST_FOLDER)
  get_filename_component(filename ${SRC_PATH} NAME)
  add_custom_command(
      TARGET copy PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory ${DEST_FOLDER}
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SRC_PATH} ${DEST_FOLDER}/${filename})
endmacro(copy_file)

macro(copy_files FILES DEST_FOLDER)
  foreach(FILENAME ${FILES})
    copy_file(${FILENAME} ${DEST_FOLDER})
  endforeach(FILENAME)
endmacro(copy_files)

macro(copy_headers module)
  copy_files("${${module}_inc}" ${inc_dir}/axle/${module}/)
endmacro(copy_headers)

macro(add_module module)
  file(GLOB ${module}_inc ${module}/*.h ${module}/*.hpp)
  file(GLOB ${module}_src ${module}/*.cc ${module}/*.cpp)
  
  # the \\ can't be changed to /
  source_group("Header Files\\${module}" FILES ${${module}_inc})
  source_group("Source Files\\${module}" FILES ${${module}_src})
  
  set(libaxle_inc ${libaxle_inc} ${${module}_inc})
  set(libaxle_src ${libaxle_src} ${${module}_src})
endmacro(add_module)

file(GLOB global_inc *.h)
set(libaxle_inc ${libaxle_inc} ${global_inc})
add_module(core)
add_module(geom)
add_module(ui)
add_module(model)
add_module(cg)
add_module(img)
#$MODULE$

add_library(${target_name} STATIC ${libaxle_src} ${libaxle_inc})

add_custom_target(copy)
copy_files("${global_inc}" ${inc_dir}/axle/)
copy_headers(core)
copy_headers(geom)
copy_headers(ui)
copy_headers(model)
copy_headers(img)
copy_headers(cg)
#$MODULE_HEADERS$

set(output "${target_name}")
set(debug_output "${output}_d")

set_target_properties(${target_name} PROPERTIES OUTPUT_NAME_DEBUG ${debug_output})
set_target_properties(${target_name} PROPERTIES OUTPUT_NAME_RELEASE ${output})

