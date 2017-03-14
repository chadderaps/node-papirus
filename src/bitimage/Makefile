
CSOURCES = bitimage.cpp \
					 bitimage_fonts.cpp \
					 test.cpp

OBJFILES = $(CSOURCES:*.cpp=*.o)

default = bitimage

$(info $(OBJFILES))
$(OBJFILES) : %.o : %.cpp
				g++ -c -o $@ $<

bitimage: $(OBJFILES)
				g++ -o $@ $^
