#include "servermessage.hpp"

ServerMessage::ServerMessage(ClientIface* sender) : m_sender(sender) {}

ServerMessage::~ServerMessage() {}
