CPP = g++
CFLAGS = -std=gnu++17 -g $(shell wx-config --cflags) -I/usr/local/include/libcamera/
LFLAGS = -g
LIBS = $(shell wx-config --libs) -L/usr/local/include/libcamera/libcamera/ -lcamera

CamTest	: CamTest.o CTFrame.o ControlWnd.o ImgCtl.o PictWnd.o
	$(CPP) -o $@ $(LFLAGS) $+ $(LIBS)

CamTest.o : CamTest.cpp CamTest.h CTFrame.h
	$(CPP) -o $@ $(CFLAGS) -c $<

CTFrame.o : CTFrame.cpp CTFrame.h ControlWnd.h PictWnd.h
	$(CPP) -o $@ $(CFLAGS) -c $<

ControlWnd.o : ControlWnd.cpp ControlWnd.h ImgCtl.h
	$(CPP) -o $@ $(CFLAGS) -c $<

ImgCtl.o : ImgCtl.cpp ImgCtl.h
	$(CPP) -o $@ $(CFLAGS) -c $<

PictWnd.o : PictWnd.cpp PictWnd.h
	$(CPP) -o $@ $(CFLAGS) -c $<
