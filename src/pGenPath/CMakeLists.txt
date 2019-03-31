#--------------------------------------------------------
# The CMakeLists.txt for:                       pGenPath
# Author(s):                              Brian Stanfield
#--------------------------------------------------------

SET(SRC
  GenPath.cpp
  GenPath_Info.cpp
  main.cpp
  Positions.cpp
)

ADD_EXECUTABLE(pGenPath ${SRC})

TARGET_LINK_LIBRARIES(pGenPath
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread
   geometry)

