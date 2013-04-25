set(inc_path ../include/axle)

file(GLOB libaxle_inc ${inc_path}/*.h)

macro(add_module module)
  set(${module}_inc_path ${inc_path}/${module})

  file(GLOB ${module}_inc ${${module}_inc_path}/*.h ${${module}_inc_path}/.hpp)
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

set(target_name axle)
add_library(${target_name} STATIC ${libaxle_src} ${libaxle_inc})

set(output "${target_name}")
set(debug_output "${output}_d")

set_target_properties(${target_name} PROPERTIES OUTPUT_NAME_DEBUG ${debug_output})
set_target_properties(${target_name} PROPERTIES OUTPUT_NAME_RELEASE ${output})

