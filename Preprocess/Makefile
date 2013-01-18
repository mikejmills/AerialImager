#Execute the uname -s to get OS name
M_ARCH := $(shell uname -s)

#Name returned by OS X from uname -s
MAC_ARCH=Darwin

CC=g++
BCFLAGS=-c -g -D_REENTRANT -Wno-deprecated 
CFLAGS=$(BCFLAGS) -I/home/mike/OpenCV2-libs/include/  -Wall -O3
LDFLAGS=-lpthread -L/home/mike/OpenCV2-libs/lib/ -lcxcore -lcv -lcvaux -lhighgui -lml

#Check Arch Mac
ifeq ($(M_ARCH), $(MAC_ARCH))  
CFLAGS=$(BCFLAGS)  -I/Users/mike/OpenCV2-libs/include/  -Wall -O3
LDFLAGS=-lpthread /usr/local/lib/libopencv_core.dylib /usr/local/lib/libopencv_highgui.dylib /usr/local/lib/libopencv_imgproc.dylib
endif

SOURCES=../Viewer/TileObj.cpp main.cpp DirHandle.cpp Preprocess.cpp connections.cpp client.cpp network.cpp particle.cpp ../include/Vector.cpp perspective.cpp ../include/common.cpp
OBJECTS=$(SOURCES:.cpp=.o) ../minilzo/minilzo.o

EXECUTABLE=main

all: $(MINILZO) $(SOURCES) $(EXECUTABLE)

# build the compression algorithm
MINILZO: ../minilzo/minilzo.c
	@echo BUILING LZO
	@g++ -c ../minilzo/minilzo.c

$(EXECUTABLE): $(OBJECTS)
	@echo LINKING $(EXECUTABLE)
	@$(CC) $(LDFLAGS) $(OBJECTS) -o $@ 

.cpp.o:
	@echo CC $(BUILD)$@
	@$(CC) $(CFLAGS) $< -o $@


clean:
	@echo CLEAN
	@rm -f $(BUILD)*.o $(BUILD)*.a $(EXECUTABLE)
	@rm -f ../Viewer/TileObj.o
	@rm -f ../minilzo/*.o ../include/Vector.o ../include/common.o


