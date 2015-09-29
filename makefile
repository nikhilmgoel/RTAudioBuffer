CXX=g++
INCLUDES=

UNAME := $(shell uname)

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

OBJS=   RtAudio.o sig_gen.o

sig_gen: $(OBJS)
	$(CXX) -o sig_gen $(OBJS) $(LIBS)

sig_gen.o: sig_gen.cpp RtAudio.h
	$(CXX) $(FLAGS) sig_gen.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

clean:
	rm -f *~ *# *.o sig_gen
