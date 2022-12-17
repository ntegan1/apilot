


all:
	echo all

routes:
	curl 'http://127.0.0.1:8081'
dcampic:
	curl -o - "http://127.0.0.1:8081/dcamera/$(ROUTE)--3" | ffmpeg -i - -ss 00:00:30  -frames:v 1 -q:v 2 out.jpg

croptest:



