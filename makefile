CXX=g++
INCLUDES=

UNAME := $(shell uname)

#ifeq ($(UNAME), Linux)
#FLAGS=-D__UNIX_JACK__ -c
#LIBS=-lasound -lpthread -ljack -lstdc++ -lm
#endif
#ifeq ($(UNAME), Darwin)
#FLAGS=-D__MACOSX_CORE__ -c
#LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
#	-framework IOKit -framework Carbon -lstdc++ -lm
#endif

ifeq ($(UNAME), Linux)
    FLAGS = -D__UNIX_JACK__ -c
    LIBS = -lasound -lpthread -ljack -lstdc++ -lm
else ifeq ($(UNAME), Darwin)
    FLAGS = -D__MACOSX_CORE__ -c
    LIBS = -framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
        -framework IOKit -framework Carbon -lstdc++ -lm
else # probably Windows
    FLAGS = -D__WINDOWS_WASAPI__ -c
    LIBS = -lwinmm -luuid -lksuser -lole32
endif

OBJS=   RtAudio.o MyHelloSine.o

MyHelloSine: $(OBJS)
	$(CXX) -o MyHelloSine $(OBJS) $(LIBS)

MyHelloSine.o: MyHelloSine.cpp RtAudio.h
	$(CXX) $(FLAGS) MyHelloSine.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

clean:
	rm -f *~ *# *.o MyHelloSine
