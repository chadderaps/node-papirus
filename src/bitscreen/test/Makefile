
CSOURCES = bitchar.cpp \
					 bitimage_fonts.cpp \
					 test.cpp

OBJFILES = $(CSOURCES:.cpp=.o)

default = bitimage

CFLAGS = -I"../../node_modules/freetype2/vendor/freetype/include" \
				 -L"../../build/Release/" -lfreetype \
				 -g

$(info $(OBJFILES))
$(OBJFILES) : %.o : %.cpp
				g++ $(CFLAGS) -c -o $@ $<

bitimage: $(CSOURCES)
				g++ $(CFLAGS) -o $@ $^
