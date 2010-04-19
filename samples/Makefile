all:	red_object_tracking

red_object_tracking: red_object_tracking.cpp
	g++ -g -I../cvBlob/ `pkg-config opencv --cflags --libs` red_object_tracking.cpp ../lib/libcvblob.a -o red_object_tracking
