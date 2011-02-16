FIND_PACKAGE(Boost COMPONENTS system thread)
FIND_PACKAGE(Bullet)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${BULLET_INCLUDE_DIRS})

ADD_LIBRARY(qntserver STATIC
  localserver.cpp
  remoteclient.cpp
  bullet/btphysics.cpp
)

TARGET_LINK_LIBRARIES(qntserver qntcore ${Boost_LIBRARIES} ${BULLET_LIBRARIES})
