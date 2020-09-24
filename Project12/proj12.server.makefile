# Make Proj12
# Tianrui Liu

GCC = g++

GCCFLAGS=  -std=c++11 -Wall

EXE = proj12.server

obj = proj12.server.o

all: $(EXE)

$(EXE):$(obj)
		$(GCC) $(GCCFLAGS) $(obj)  -o  $(EXE)
proj12.server.o: proj12.server.c
		$(GCC)  $(GCCFLAGS) -c proj12.server.c

.PHONY : clean all
clean:
	rm -f $(EXE)  $(obj)
