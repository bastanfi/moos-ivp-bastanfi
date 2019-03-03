#--------------------------------------------------------
# The CMakeLists.txt for:                       pCommunicationAngle_bastanfi
# Author(s):                              Brian Stanfield
#--------------------------------------------------------

SET(SRC
  CommunicationAngle_bastanfi.cpp
  CommunicationAngle_bastanfi_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pCommunicationAngle_bastanfi ${SRC})

TARGET_LINK_LIBRARIES(pCommunicationAngle_bastanfi
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

