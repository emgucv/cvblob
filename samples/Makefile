all:	red_object_tracking

red_object_tracking: red_object_tracking.cpp
	g++ -g -I../cvBlob/ `pkg-config --libs opencv` red_object_tracking.cpp ../lib/libcvblob.a -o red_object_tracking
