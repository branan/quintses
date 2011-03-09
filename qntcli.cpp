#include "client/localclient.hpp"
#include "client/remoteserver.hpp"

int main(int argc, char** argv) {
  RemoteServer *s;
  if(argc >= 2)
    s = new RemoteServer(argv[1]);
  else
    s = new RemoteServer;
  LocalClient cli(s);
  return cli.mainloop();
}