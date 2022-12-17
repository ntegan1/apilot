 #ffmpeg -r 20 -f hevc -i dcamera.hevc -c copy -vtag hvc1 -map 0 out.mp4

 #https://video.stackexchange.com/questions/4563/how-can-i-crop-a-video-with-ffmpeg
 # ffmpeg -i out.mp4 -filter:v "crop=1280:720:0:0" outcrop.mp4


 # overlay
 #https://stackoverflow.com/questions/14676517/overlaying-multiple-videos-with-ffmpeg

 # multiple
 #https://stackoverflow.com/questions/6195872/applying-multiple-filters-at-once-with-ffmpeg
 #https://stackoverflow.com/questions/36962963/how-to-trim-crop-and-add-overlay-in-a-single-command-using-ffmpeg

 # dcamcrop
 ## shotcut
 top=561
 bottom=157
 left=1212
 right=186


 ow=$((1928 - left - right))
 oh=$((1208 - top - bottom))
 x=${left}
 y=${top}
 #ffmpeg -i out.mp4 -filter:v "crop=${ow}:${oh}:${x}:${y}" outcrop2.mp4
 # position = 1324, 675 zoom 36.5 %?
 

 # overlay outcrop2 ontop of oute

 #ffmpeg -i oute.mp4 -i outcrop2.mp4 -filter_complex "overlay=main_w-overlay_w-10:main_h-overlay_h-10" outoverlay.mp4

 # speecifying inputs
 #ffmpeg -i oute.mp4 -i outcrop2.mp4 -filter_complex "[0:v][1:v] overlay=main_w-overlay_w-10:main_h-overlay_h-10" outoverlay.mp4

## multi filter
#ffmpeg -i out.mp4 -filter:v "crop=${ow}:${oh}:${x}:${y}" outcrop2.mp4
#ffmpeg -i oute.mp4 -i out.mp4 \
#  -filter_complex "[1:v]crop=${ow}:${oh}:${x}:${y}[out];[0:v][out] overlay=main_w-overlay_w-10:main_h-overlay_h-10" outoverlay.mp4


# get one frame
# https://stackoverflow.com/questions/27568254/how-to-extract-1-screenshot-for-a-video-with-ffmpeg-at-a-given-time
ffmpeg -ss 00:00:30 -i out.mp4 -frames:v 1 -q:v 2 output.jpg



 eye of gnome
