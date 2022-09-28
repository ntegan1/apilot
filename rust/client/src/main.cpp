#include <stdio.h>
#include <sys/resource.h>

#include <chrono>
#include <thread>
#include <vector>

#include "cereal/messaging/messaging.h"
#include "common/swaglog.h"
#include "common/timing.h"
#include "common/util.h"
//#include "shmClient.h"

extern "C" {
void *init_mem();
void deinit_mem(void*);
void print_value(void*);
int8_t get_value(void*);
}

class SharedMemory {
public:
  SharedMemory()
    : theptr(init_mem())
  {}
  ~SharedMemory()
  { deinit_mem(theptr); theptr = NULL; }

  int8_t get() {return (int8_t) get_value(theptr); }
private:
  void * theptr{NULL};
};

#define msgGen(x) MessageBuilder msg; msg.initEvent().initNewService().setSliderone(x);
ExitHandler do_exit;

//clang++ -o rust/client/client -Wl,--as-needed -Wl,--no-undefined -Wl,-rpath=/data/openpilot/third_party/acados/larch64/lib -Wl,-rpath=/usr/local/lib rust/client/src/main.o -L/usr/local/lib -L/usr/lib -L/system/vendor/lib64 -Lthird_party/acados/larch64/lib -Lthird_party/snpe/larch64 -Lthird_party/libyuv/larch64/lib -L/usr/lib/aarch64-linux-gnu -Lcereal -Lthird_party -Lopendbc/can -Lselfdrive/boardd -Lcommon -Lrust/client/target/release common/libcommon.a -ljson11 cereal/libcereal.a cereal/libmessaging.a -lcapnp -lzmq -lkj -lssl -lcrypto -lpthread -lrt -ldl -lshmCli

const char * service = "newService";
//scons: done reading SConscript files.
//scons: Building targets ...
//clang++ -o rust/client/src/main.o -c -std=c++1z -DQCOM2 -mcpu=cortex-a57 -DSWAGLOG="\"common/swaglog.h\"" -g -fPIC -O2 -Wunused -Werror -Wshadow -Wno-unknown-warning-option -Wno-deprecated-register -Wno-register -Wno-inconsistent-missing-o
//verride -Wno-c99-designator -Wno-reorder-init-list -Wno-error=unused-but-set-variable -DQCOM2 -mcpu=cortex-a57 -DSWAGLOG="\"common/swaglog.h\"" -Wno-error -Wno-shadow -Ithird_party/opencl/include -I. -Ithird_party/acados/include -Ithird_pa
//rty/acados/include/blasfeo/include -Ithird_party/acados/include/hpipm/include -Ithird_party/catch2/include -Ithird_party/libyuv/include -Ithird_party/json11 -Ithird_party/curl/include -Ithird_party/libgralloc/include -Ithird_party/android_
//frameworks_native/include -Ithird_party/android_hardware_libhardware/include -Ithird_party/android_system_core/include -Ithird_party/linux/include -Ithird_party/snpe/include -Ithird_party/mapbox-gl-native-qt/include -Ithird_party/qrcode -I
//third_party -Icereal -Iopendbc/can -I/data/boost/boost_1_80_0 rust/client/src/main.cpp
//clang++ -o rust/client/client -Wl,--as-needed -Wl,--no-undefined -Wl,-rpath=/data/openpilot/third_party/acados/larch64/lib -Wl,-rpath=/usr/local/lib rust/client/src/main.o -L/usr/local/lib -L/usr/lib -L/system/vendor/lib64 -Lthird_party/ac
//ados/larch64/lib -Lthird_party/snpe/larch64 -Lthird_party/libyuv/larch64/lib -L/usr/lib/aarch64-linux-gnu -Lcereal -Lthird_party -Lopendbc/can -Lselfdrive/boardd -Lcommon -Lrust/client/target/release common/libcommon.a -ljson11 cereal/libc
//ereal.a cereal/libmessaging.a -lcapnp -lzmq -lkj -lssl -lcrypto -lpthread -lrt -ldl -lshmCli
///usr/bin/ld: skipping incompatible /usr/lib/libgcc_s.so.1 when searching for libgcc_s.so.1
///usr/bin/ld: skipping incompatible /usr/lib/libgcc_s.so.1 when searching for libgcc_s.so.1
///usr/bin/ld: rust/client/target/release/libshmCli.a(std-768f64e43b4091ca.std.311f8908-cgu.0.rcgu.o): undefined reference to symbol 'dlsym@@GLIBC_2.17'
///usr/bin/ld: /usr/lib/aarch64-linux-gnu/libdl.so: error adding symbols: DSO missing from command line
//clang: error: linker command failed with exit code 1 (use -v to see invocation)
//scons: *** [rust/client/client] Error 1
//scons: building terminated because of errors.

// freq to millis
unsigned long long ftom (float f) { return (unsigned long long)(1.0 / f * 1000.0); }
void sMil(unsigned long long millis) {
  std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}
void sHz(float f) { sMil(ftom(f)); }
void sendMsg (PubMaster &pm, MessageBuilder &msg) {
  pm.send(service, msg);
}
void sendVal (PubMaster &pm, unsigned char val) {
  msgGen(val);
  sendMsg(pm, msg);
}
void pNoClient(PubMaster &pm) {
  sendVal(pm, 0);
  //eprintf("[noclient]\n");
}

int sensor_loop() {
  SharedMemory mem;
  //float noShmServerHz = 0.45;
  PubMaster pm({service});
  //string name = "shm";
  //for(;;!do_exit) { try { ShmClient client(name); break; } catch (std::exception &e) { pNoClient(pm); sHz(noShmServerHz); } }
  //ShmClient client("shm");
  float update_frequency = 20.0;
  auto update_millis = ftom(update_frequency);

  // remap shm every few seconds
  //float remap_frequency = 0.28;
  //auto remap_millis = ftom(remap_frequency);
  //std::chrono::steady_clock::time_point remap_begin = std::chrono::steady_clock::now();

  while (!do_exit) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    MessageBuilder msg;
    auto new_service = msg.initEvent().initNewService();

    int8_t byte = mem.get();
    new_service.setSliderone(byte);

    pm.send(service, msg);


    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    //if (std::chrono::duration_cast<std::chrono::milliseconds>(end - remap_begin).count() >= remap_millis) {
    //  for (;;!do_exit) { try { client.remap(); break; } catch (std::exception &e) { pNoClient(pm); sHz(noShmServerHz); } }
    //    remap_begin = end;
    //  }
    //  //{ remap_begin = end; client.remap(); client.printDebugInfo(); }
    std::this_thread::sleep_for(std::chrono::milliseconds(update_millis) - (end - begin));
  }
  return 0;
}

int main (int argc, char **argv) {
  setpriority(PRIO_PROCESS, 0, -18);
  return sensor_loop();
}
