FIND_PACKAGE(OpenGL REQUIRED)

ADD_LIBRARY(qntclient STATIC
  localclient.cpp
  localclient_${QNT_PLATFORM}.cpp
)

TARGET_LINK_LIBRARIES(qntclient qntcore qntserver ${OPENGL_LIBRARIES})