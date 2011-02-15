#include "servermsg.hpp"

ServerMsg::ServerMsg(ClientIface* sender) : m_sender(sender) {}

ServerMsg::~ServerMsg() {}
