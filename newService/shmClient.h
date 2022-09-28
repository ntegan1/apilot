#pragma once
#include "inc.h"
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
using namespace boost::interprocess;

class ShmClient {
  string name_;
  shared_memory_object shm_;
  mapped_region region_;
  unsigned char * address_{NULL};
  size_t size_{0};
public:
  ShmClient(string name)
    : name_(name) {
    shm_ = shared_memory_object(open_only, name.c_str(), read_only);
    region_ = mapped_region(shm_, read_only);
    size_ = region_.get_size();
    address_ = (unsigned char*) region_.get_address();
  }
  void printDebugInfo() {
    eprintf("region: %p, size: %zu\n", address_, size_);
  }
  unsigned char getByte() {
    return address_[0];
  }
  void remap() {
    shm_ = shared_memory_object(open_only, name_.c_str(), read_only);
    region_ = mapped_region(shm_, read_only);
    size_ = region_.get_size();
    address_ = (unsigned char*) region_.get_address();
  }
};
