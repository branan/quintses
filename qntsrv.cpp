#include "server/localserver.hpp"

int main(int , char**) {
  LocalServer s;
  return s.waitForTermination();
}
