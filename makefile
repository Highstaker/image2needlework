CC = g++
INCLUDES = -I/usr/include/freetype2
LIBRARIES = -lfreetype -lz
SOURCE = $(wildcard *.cpp)
DEST = $(patsubst %.cpp,%,$(SOURCE))

all: $(DEST)

%: %.cpp
	rm -f /mnt/ramtemp/$@
	 $(CC) $< -o /mnt/ramtemp/$@ $(LIBRARIES) $(INCLUDES)
	 
