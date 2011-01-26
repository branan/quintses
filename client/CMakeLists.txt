ADD_LIBRARY(qntclient STATIC
  localclient.cpp
)

TARGET_LINK_LIBRARIES(qntclient qntserver)