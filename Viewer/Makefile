#Execute the uname -s to get OS name
M_ARCH := $(shell uname -s)

#Name returned by OS X from uname -s
MAC_ARCH=Darwin

GLIB=/usr/X11R6/lib

CC=g++
BCFLAGS=-c -g -D_REENTRANT -Wno-deprecated -DVIEWER
CFLAGS=$(BCFLAGS) -I/home/mike/OpenCV2-libs/include/opencv -O3
LDFLAGS=-lpthread -L/home/mike/OpenCV2-libs/lib/ -lhighgui -L$(GLIB) -lglut -lGLU

#Check Arch Mac
ifeq ($(M_ARCH), $(MAC_ARCH))  
	CFLAGS=$(BCFLAGS) -I/Users/mike/OpenCV2-libs/include/ -Wall  -O3
	LDFLAGS=-lpthread /usr/local/lib/libopencv*.dylib /usr/local/lib/libopencv_highgui.dylib /usr/local/lib/libopencv_imgproc.dylib -framework OpenGL -framework GLUT -framework Foundation
endif


SOURCES=../include/threadpool.cpp ../include/Vector.cpp ../include/common.cpp matrix.cpp texture.cpp gl.cpp TileObj.cpp server.cpp connections.cpp vfs.cpp MainList.cpp Display.cpp main.cpp GeoCoord.cpp Object.cpp DisplayThread.cpp cameras.cpp ViewRec.cpp FlagGUI.cpp
  
OBJECTS=$(SOURCES:.cpp=.o) ../minilzo/minilzo.o

EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@echo LINKING $(EXECUTABLE)
	@$(CC) $(OBJECTS) -o $@  $(LDFLAGS) 

.cpp.o:
	@echo CC $(BUILD)$@
	@$(CC) $(CFLAGS) $< -o $@


clean:
	@echo CLEAN
	@rm -f $(BUILD)*.o $(BUILD)*.a $(EXECUTABLE)
	@rm -f ../minilzo/*.o ../include/common.o ../include/Vector.o ../include/serialize.o ../include/threadpool.o
