# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\appeindopdracht_CPP_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appeindopdracht_CPP_autogen.dir\\ParseCache.txt"
  "appeindopdracht_CPP_autogen"
  )
endif()
