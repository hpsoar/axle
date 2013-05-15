set(inc_path ../include/axle)
set(target_name axle)

file(GLOB libaxle_inc ${inc_path}/*.h)

macro(copy_file FILENAME DEST_FOLDER)
  message(STATUS "filename=${FILENAME}")
  message(STATUS "dest folder=${DEST_FOLDER}")
  add_custom_command(
      TARGET ${target_name} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${FILENAME} ${DEST_FOLDER})
endmacro(copy_file)

macro(copy_headers module)
  file(GLOB ${module}_headers ${module}/*.h)
  #copy_file(${module}_headers ${inc_path}/${module})
  foreach(FILENAME ${${module}_headers})
    copy_file(${FILENAME} ${inc_dir}/axle/${module})
  endforeach(FILENAME)
endmacro(copy_headers)

macro(add_module module)
  set(module_inc_path ${inc_path}/${module})
  set(${module}_inc_path ${inc_path}/${module})

  file(GLOB ${module}_inc ${module_inc_path}/*.h ${module_inc_path}/.hpp)
  file(GLOB ${module}_src ${module}/*.cc ${module}/*.cpp)
  
  # the \\ can't be changed to /
  source_group("Header Files\\${module}" FILES ${${module}_inc})
  source_group("Source Files\\${module}" FILES ${${module}_src})
  
  set(libaxle_inc ${libaxle_inc} ${${module}_inc})
  set(libaxle_src ${libaxle_src} ${${module}_src})
endmacro(add_module)

add_module(core)
add_module(geom)
add_module(ui)
add_module(model)
add_module(cg)
#$MODULE$

add_library(${target_name} STATIC ${libaxle_src} ${libaxle_inc})
#copy_headers(core)
#copy_headers(geom)
#copy_headers(ui)
#copy_headers(model)
copy_headers(cg)
#$MODULE_HEADERS$

set(output "${target_name}")
set(debug_output "${output}_d")

set_target_properties(${target_name} PROPERTIES OUTPUT_NAME_DEBUG ${debug_output})
set_target_properties(${target_name} PROPERTIES OUTPUT_NAME_RELEASE ${output})

