#include <sys/resource.h>

#include "inc.h"
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
using namespace boost::interprocess;
#include "config.h"
#include "listener.h"

#define IOC net::io_context

// unused
class Shared {
  string name_;
  std::size_t size_;
  shared_memory_object shm_;
  mapped_region region_;
  unsigned char * address_;
public:
  ~Shared() { shared_memory_object::remove(name_.c_str()); }
  Shared(const char *name, std::size_t size)
    : name_(name)
    , size_(size)
    , shm_(create_only, name, read_write) {
      shm_.truncate(size);
      region_ = mapped_region(shm_, read_write);
      address_ = (decltype(address_)) region_.get_address();
  }
  void setByte(unsigned char &c) {
    if (size_ < 1) { return; }
    *address_ = c;
  }

};


int main(int argc, char *argv[]) {
  setpriority(PRIO_PROCESS, 0, -18);
  IOC ioc;
  ssl::context ctx{ssl::context::tlsv12};
  try {
    shared_memory_object::remove("shm");
  } catch (std::exception &e) {}

  SignalHook s(ioc);
  // This holds the self-signed certificate used by the server
  load_server_certificate(ctx);
  std::make_shared<listener>(
    ioc, ctx, tcp::endpoint{net::ip::make_address(SERVER_BIND_ADDRESS), SERVER_LISTEN_PORT}
  )->run();
  ioc.run();
  return 0;
}
