#--------------------------------------------------------
# The CMakeLists.txt for:                       pPointAssign
# Author(s):                              Brian Stanfield
#--------------------------------------------------------

SET(SRC
  PointAssign.cpp
  PointAssign_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pPointAssign ${SRC})

TARGET_LINK_LIBRARIES(pPointAssign
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread
   geometry)
