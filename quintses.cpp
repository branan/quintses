#include "client/localclient.hpp"
#include "server/localserver.hpp"

int main(int, char**){
  LocalServer *srv = new LocalServer;
  LocalClient cli(srv);
  srv->makeClientPrivileged(&cli);
  return cli.mainloop();
}
