#include <sys/resource.h>

#include <chrono>
#include <thread>
#include <vector>

#include "cereal/messaging/messaging.h"
#include "common/swaglog.h"
#include "common/timing.h"
#include "common/util.h"
#include "shmClient.h"

//{ MessageBuilder msg; auto new_service = msg.initEvent().initNewService(); new_service.setSliderone(middle); pm.send("newService", msg); }
#define msgGen(x) MessageBuilder msg; msg.initEvent().initNewService().setSliderone(x);

ExitHandler do_exit;
const char * service = "newService";
const char middle_char = 0;
const unsigned char middle = middle_char;

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
  sendVal(pm, middle);
  eprintf("[noclient]\n");
}

int sensor_loop() {
  float noShmServerHz = 0.45;
  PubMaster pm({service});
  string name = "shm";
  for(;;!do_exit) { try { ShmClient client(name); break; } catch (std::exception &e) { pNoClient(pm); sHz(noShmServerHz); } }
  ShmClient client("shm");
  float update_frequency = 20.0;
  auto update_millis = ftom(update_frequency);

  // remap shm every few seconds
  float remap_frequency = 0.28;
  auto remap_millis = ftom(remap_frequency);
  std::chrono::steady_clock::time_point remap_begin = std::chrono::steady_clock::now();

  while (!do_exit) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    MessageBuilder msg;
    auto new_service = msg.initEvent().initNewService();

    unsigned char byte = client.getByte();
    //byte = byte <= 127 ? 128 - byte : byte;
    new_service.setSliderone(byte);

    pm.send(service, msg);


    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(end - remap_begin).count() >= remap_millis) {
      for (;;!do_exit) { try { client.remap(); break; } catch (std::exception &e) { pNoClient(pm); sHz(noShmServerHz); } }
        remap_begin = end;
      }
      //{ remap_begin = end; client.remap(); client.printDebugInfo(); }
    std::this_thread::sleep_for(std::chrono::milliseconds(update_millis) - (end - begin));
  }
  return 0;
}

int main(int argc, char *argv[]) {
  setpriority(PRIO_PROCESS, 0, -18);
  return sensor_loop();
}
