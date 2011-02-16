FIND_PACKAGE(OpenAL REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

INCLUDE_DIRECTORIES(${OPENAL_INCLUDE_DIR})

ADD_LIBRARY(qntclient STATIC
  localclient.cpp
  localclient_${QNT_PLATFORM}.cpp
  remoteserver.cpp
  openal/alaudio.cpp
  openal/streamingfile.cpp
  openal/streamingwav.cpp
  opengl/glloader.cpp
  opengl/glloader_${QNT_PLATFORM}.cpp
  opengl/glrender.cpp
)

TARGET_LINK_LIBRARIES(qntclient qntcore qntserver ${OPENGL_LIBRARIES} ${OPENAL_LIBRARY})