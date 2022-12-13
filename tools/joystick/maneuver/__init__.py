#!/usr/bin/env python3
import json
import os

whereami = str(os.path.dirname(os.path.abspath(__file__)))

maneuvers_directory = whereami + "/maneuvers"

def available_maneuver_files():
  all_files = os.listdir(maneuvers_directory)
  files = [f for f in all_files if f[f.rfind(".")+1:] == "json"]
  return files

class Maneuver:
  def get_wolfram_alpha_paste(self):
    out = "plot points["
    for point in self.object["points"]:
      #plot points[(-5/2, 18),(-2, 14),(-1, 39/10),(0, 1/2),(1, 19/10),(2, 5),(5/2, 17/2)]
      out += ("(" + str(point[0]) + "," + str(point[1]) + "),")
    out = out[:-1] + "]"
    return out
  def __init__(self, file_name):
    self.file_name = file_name
    self.object = json.load(open(maneuvers_directory + "/" + file_name))

a = Maneuver("0.json")
print(a.object)
print(a.get_wolfram_alpha_paste())

