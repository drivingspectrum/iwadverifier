SOURCES = src/dlg_main.cpp \
 src/main.cpp \
 src/wxMD5/md5.cpp \
 src/wxMD5/wxMD5.cpp
 
HEADERS = res/xrc_resource.h \
 src/dlg_main.h \
 src/main.h \
 src/wxMD5/md5.h \
 src/wxMD5/wxMD5.h 

CC = g++
LD = g++

WXCONFIG = wx-config
WXRC = wxrc
CFLAGS = $(shell $(WXCONFIG) --cflags) -g
LFLAGS = $(shell $(WXCONFIG) --libs) -g
INCLUDES = -I./src -I./src/wxMD5 -I./res
RESOURCES = res/xrc_resource.xrc

OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

iwadverifier: $(OBJECTS)
	$(LD) $(OBJECTS) -o iwadverifier $(LFLAGS)

%.o: %.cpp $(HEADERS)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES)

res/xrc_resource.h: $(RESOURCES)
	$(WXRC) -c -o $@ $(RESOURCES)

clean:
	rm -rf iwadverifier
	rm -rf $(OBJECTS)
