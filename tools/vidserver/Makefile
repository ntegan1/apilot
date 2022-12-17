
ROUTE ?= 2022-12-16--15-36-42

dcamw=1928
dcamh=1208

top=561
bottom=157
left=1212
right=186


ow=$(shell echo $$(($(dcamw) - $(left) - $(right))))
oh=$(shell echo $$(($(dcamh) - $(top) - $(bottom))))
x=$(left)
y=$(top)

all:
	make dcampic

routes:
	curl 'http://127.0.0.1:8081'
dcampic:
	curl -o - "http://127.0.0.1:8081/dcamera/$(ROUTE)--3" | \
		head -n 100000 | \
		ffmpeg -y -i - -ss 00:00:02 -filter:v "crop=$(ow):$(oh):$(x):$(y)" -frames:v 1 -q:v 2 out.jpg
	! timeout 3 eog out.jpg
	$(RM) out.jpg

