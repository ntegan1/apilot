#!/usr/bin/env python3
import os
import subprocess

from flask import Flask, Response, request
#from selfdrive.loggerd.config import ROOT
from selfdrive.loggerd.uploader import listdir_by_creation
from tools.lib.route import SegmentName

ROOT = "/home/ntegan/2022_12_16_tacov1/data/"

def is_valid_segment(segment):
  try:
    segment_to_segment_name(ROOT, segment)
    return True
  except AssertionError:
    return False

def segment_to_segment_name(data_dir, segment):
  fake_dongle = "ffffffffffffffff"
  return SegmentName(str(os.path.join(data_dir, fake_dongle + "|" + segment)))

def all_segment_names():
  segments = []
  for segment in listdir_by_creation(ROOT):
    try:
      segments.append(segment_to_segment_name(ROOT, segment))
    except AssertionError:
      pass
  return segments

def all_routes():
  segment_names = all_segment_names()
  route_names = [segment_name.route_name for segment_name in segment_names]
  route_times = [route_name.time_str for route_name in route_names]
  unique_routes = list(set(route_times))
  return unique_routes

def segments_in_route(route):
  segment_names = [segment_name for segment_name in all_segment_names() if segment_name.time_str == route]
  segments = [segment_name.time_str + "--" + str(segment_name.segment_num) for segment_name in segment_names]
  return segments

def ffmpeg_mp4_concat_wrap_process_builder(file_list, cameratype, chunk_size=1024*512):
  command_line = ["ffmpeg"]
  if not cameratype == "qcamera":
    command_line += ["-f", "hevc"]
  command_line += ["-r", "20"]
  command_line += ["-i", "concat:" + file_list]
  command_line += ["-c", "copy"]
  command_line += ["-map", "0"]
  if not cameratype == "qcamera":
    command_line += ["-vtag", "hvc1"]
  command_line += ["-f", "mp4"]
  command_line += ["-movflags", "empty_moov"]
  command_line += ["-"]
  return subprocess.Popen(
    command_line, stdout=subprocess.PIPE,
    bufsize=chunk_size
  )
def ffmpeg_mp4_wrap_process_builder(filename):
  """Returns a process that will wrap the given filename
     inside an mp4 container, for easier playback by browsers
     and other devices. Primary use case is streaming segment videos
     to the vidserver tool.

     filename is expected to be a pathname to one of the following
       /path/to/a/qcamera.ts
       /path/to/a/dcamera.hevc
       /path/to/a/ecamera.hevc
       /path/to/a/fcamera.hevc
  """
  basename = filename.rsplit("/")[-1]
  extension = basename.rsplit(".")[-1]
  command_line = ["ffmpeg"]
  if extension == "hevc":
    command_line += ["-f", "hevc"]
  command_line += ["-r", "20"]
  command_line += ["-i", filename]
  command_line += ["-c", "copy"]
  command_line += ["-map", "0"]
  if extension == "hevc":
    command_line += ["-vtag", "hvc1"]
  command_line += ["-f", "mp4"]
  command_line += ["-movflags", "empty_moov"]
  command_line += ["-"]
  return subprocess.Popen(
    command_line, stdout=subprocess.PIPE
  )

app = Flask(__name__,)

@app.route("/full/<cameratype>/<route>")
def full(cameratype, route):
  chunk_size = 1024 * 512 # 5KiB
  #if not is_valid_route(route):
  #  return "invalid route"
  file_name = cameratype + (".ts" if cameratype == "qcamera" else ".hevc")
  vidlist = "|".join(ROOT + "/" + segment + "/" + file_name for segment in segments_in_route(route))
  def generate_buffered_stream():
    with ffmpeg_mp4_concat_wrap_process_builder(vidlist, cameratype, chunk_size) as process:
      for chunk in iter(lambda: process.stdout.read(chunk_size), b""):
        yield bytes(chunk)
  return Response(generate_buffered_stream(), status=200, mimetype='video/mp4')

@app.route("/<cameratype>/<segment>")
def fcamera(cameratype, segment):
  if not is_valid_segment(segment):
    return "invalid segment"
  file_name = ROOT + "/" + segment + "/" + cameratype + (".ts" if cameratype == "qcamera" else ".hevc")

  return Response(ffmpeg_mp4_wrap_process_builder(file_name).stdout.read(), status=200, mimetype='video/mp4')

@app.route("/<route>")
def route(route):
  if len(route) != 20:
    return "route not found"

  if str(request.query_string) == "b''":
    query_segment = str("0")
    query_type = "qcamera"
  else:
    query_segment = (str(request.query_string).split(","))[0][2:]
    query_type = (str(request.query_string).split(","))[1][:-1]

  links = ""
  segments = ""
  for segment in segments_in_route(route):
    links += "<a href='"+route+"?"+segment.split("--")[2]+","+query_type+"'>"+segment+"</a><br>"
    segments += "'"+segment+"',"
  return """<html>
  <body>
    <video id="video" width="320" height="240" controls autoplay="autoplay" style="background:black">
    </video>
    <br><br>
    current segment: <span id="currentsegment"></span>
    <br>
    current view: <span id="currentview"></span>
    <br>
    <a download=\""""+route+"-"+ query_type + ".mp4" + """\" href=\"/full/"""+query_type+"""/"""+route+"""\">download full route """ + query_type + """</a>
    <br><br>
    <a href="\\">back to routes</a>
    <br><br>
    <a href=\""""+route+"""?0,qcamera\">qcamera</a> -
    <a href=\""""+route+"""?0,fcamera\">fcamera</a> -
    <a href=\""""+route+"""?0,dcamera\">dcamera</a> -
    <a href=\""""+route+"""?0,ecamera\">ecamera</a>
    <br><br>
    """+links+"""
  </body>
    <script>
    var video = document.getElementById('video');
    var tracks = {
      list: ["""+segments+"""],
      index: """+query_segment+""",
      next: function() {
        if (this.index == this.list.length - 1) this.index = 0;
        else {
            this.index += 1;
        }
      },
      play: function() {
        return ( \""""+query_type+"""/" + this.list[this.index] );
      }
    }
    video.addEventListener('ended', function(e) {
      tracks.next();
      video.src = tracks.play();
      document.getElementById("currentsegment").textContent=video.src.split("/")[4];
      document.getElementById("currentview").textContent=video.src.split("/")[3];
      video.load();
      video.play();
    });
    video.src = tracks.play();
    document.getElementById("currentsegment").textContent=video.src.split("/")[4];
    document.getElementById("currentview").textContent=video.src.split("/")[3];
    </script>
</html>
"""

@app.route("/")
def index():
  result = ""
  for route in all_routes():
    result += "<a href='"+route+"'>"+route+"</a><br>"
  return result

def main():
  app.run(host="0.0.0.0", port="8081")

if __name__ == '__main__':
  main()
