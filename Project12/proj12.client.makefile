# Make Proj12
# Tianrui Liu

GCC = g++

GCCFLAGS=  -std=c++11 -Wall

EXE = proj12.client

obj = proj12.client.o

all: $(EXE)

$(EXE):$(obj)
		$(GCC) $(GCCFLAGS) $(obj)  -o  $(EXE)
proj12.client.o: proj12.client.c
		$(GCC)  $(GCCFLAGS) -c proj12.client.c

.PHONY : clean all
clean:
	rm -f $(EXE)  $(obj)
