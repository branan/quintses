#include "client/localclient.hpp"
#include "client/remoteserver.hpp"

int main(int, char** argv) {
  RemoteServer *s = new RemoteServer(argv[1]);
  LocalClient cli(s);
  return cli.mainloop();
}