ADD_LIBRARY(qntcore STATIC
  iface_vtable.cpp
  messages/client/clientmsg.cpp
  messages/client/clientaddobjectmsg.cpp
  messages/client/clientdelobjectmsg.cpp
  messages/client/clienttransobjectmsg.cpp
  messages/server/inputmsg.cpp
  messages/server/loadplayermsg.cpp
  messages/server/servermsg.cpp
  messages/server/shutdownmsg.cpp
)
