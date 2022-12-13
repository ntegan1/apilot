#!/usr/bin/env python3
import datetime
import json
import atexit
import os
from common.numpy_fast import interp
from common.conversions import Conversions as CV


# TODO: use maneuvercontroller in longitudinal planner

# TODO: make script that uses mem to start a maneuver or something

whereami = str(os.path.dirname(os.path.abspath(__file__)))

maneuvers_directory = whereami + "/maneuvers"

def available_maneuver_files():
  all_files = os.listdir(maneuvers_directory)
  files = [f for f in all_files if f[f.rfind(".")+1:] == "json"]
  return files

class ManeuverController:
  maneuvering = False
  maneuver = None
  t = 0.
  start_time = None
  def update(self, vcruise):
    should_start_maneuver = not self.maneuvering and self.mem.maneuver_requested()
    should_stop_maneuver = self.maneuvering and not self.mem.maneuver_requested()
    if not self.maneuvering and not should_start_maneuver:
      return vcruise

    if should_start_maneuver:
      self.t = 0.
      self.start_time = datetime.datetime.now()
      file_name = str(self.mem.maneuver_num()) + ".json"
      self.maneuver = Maneuver(file_name)
      self.maneuvering = True
    elif should_stop_maneuver:
      self.maneuvering = False
      self.maneuver = None
      self.t = 0.
      self.start_time = None
      # TODO
      # do/finish this and see what else i missed and put it into the car
      # start over ssh for now

    dt = datetime.datetime.now() - self.start_time
    self.t = float(dt.seconds) + float(dt.microseconds) / 1.0e6
    return self.maneuver.get_velocity_kph(self.t)
  def set_finished(self):
    self.maneuvering = False
    self.maneuver = None
  def request_maneuver_start(self, maneuver):
    self.maneuver = maneuver
    self.maneuvering = True
  def __init__(self):
    self.mem = Mem(autounlink=True)

class Maneuver:
  def get_wolfram_alpha_paste(self):
    out = "plot points["
    for point in self.object["points"]:
      out += ("(" + str(point[0]) + "," + str(point[1]) + "),")
    out = out[:-1] + "]"
    return out
  def get_velocity_kph(self, t):
    if self.should_interpolate:
      return interp(t, self.times, self.speeds)
    else:
      return "ffff"
  def __points_to_interpables(self):
    times = []
    speeds_kph = []
    conversions = {
      "mph": CV.MPH_TO_KPH,
      "kph": 1.,
      "mps": CV.MS_TO_KPH,
      "m/s": CV.MS_TO_KPH,
      "si": CV.MS_TO_KPH,
    }
    unit = self.object["velocity_unit"]
    conversion = conversions[unit]
    for point in self.object["points"]:
      times.append(float(point[0]))
      speeds_kph.append(float(point[1]) * conversion)
    self.times = times
    self.speeds = speeds_kph
  def __init__(self, file_name):
    self.file_name = file_name
    self.object = json.load(open(maneuvers_directory + "/" + file_name))
    self.should_interpolate = bool(self.object["should_interpolate"])
    self.__points_to_interpables()

class Mem:
  __mem = None
  name = "maneuvermem"
  size = 2
  def maneuver_requested(self):
    return bool(buf[0])
  def maneuver_num(self):
    return buf[1]
  def maneuver_request(self, maneuver_num):
    self.buf[1] = maneuver_num
    self.buf[0] = 1
  def maneuver_finish(self):
    self.__mem.buf[0:size] = bytearray([0 for _ in range(size)])
  def __create_or_connect(self):
    self.__mem = shared_memory.SharedMemory(name=self.name, create=True, size=self.size)
  def __cleanup(self):
    self.__mem.close()
    if self.__shouldunlink:
      self.__mem.unlink()
  def __init__(self, autounlink=False):
    self.__shouldunlink = autounlink
    self.__create_or_connect()
    self.__mem.buf[0:size] = bytearray([0 for _ in range(size)])
    atexit.register(self.__cleanup)
