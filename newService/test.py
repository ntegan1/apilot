#!/usr/bin/env python3

from cereal import log
from cereal import messaging
import sys
import time
import signal
import serial
import struct
import requests
import urllib.parse
from datetime import datetime
from typing import List, Optional

#  pm = messaging.PubMaster(['ubloxRaw'])
#  while True:
#    dat = pigeon.receive()
#    if len(dat) > 0:
#      if dat[0] == 0x00:
#        cloudlog.warning("received invalid data from ublox, re-initing!")
#        initialize_pigeon(pigeon)
#        continue
#
#      # send out to socket
#      msg = messaging.new_message('ubloxRaw', len(dat))
#      msg.ubloxRaw = dat[:]
#      pm.send('ubloxRaw', msg)
from time import sleep
def main():
  nsm = None
  while 1:
    service = "newService"
    if nsm == None:
      nsm = messaging.SubMaster([service])
    nsm.update(0)
    slider = nsm[service].sliderone
    if (slider > 127):
      slider = 128 - slider
    print(slider)
    sleep(1)

  poller = messaging.Poller()
  messaging.sub_sock("newService", poller)
  while 1:
    polld = poller.poll(100)
    for sock in polld:
      msg = sock.receive()
      evt = log.Event.from_bytes(msg)
      f = evt.newService.sliderone
      try:
        print(int(128 - f))
      except UnicodeDecodeError:
        w = evt.which()
        s = f"( logMonoTime {evt.logMonoTime} \n  {w} = "
        s += str(evt.__getattr__(w))
        s += f"\n  valid = {evt.valid} )"
        print(s)


if __name__ == "__main__":
  main()
