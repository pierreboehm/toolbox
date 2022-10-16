GCC_PATH := "/Users/pierreboehm/Downloads/aarch64-unknown-linux-gnu/bin/"
GCC_COMPILER := "aarch64-unknown-linux-gnu-gcc"
GCC_STRIPPER := "aarch64-unknown-linux-gnu-strip"

GCC := $(GCC_PATH)$(GCC_COMPILER)
STP := $(GCC_PATH)$(GCC_STRIPPER)

objects = last.o net.o

last.o : last.c
	$(GCC) -c last.c
net.o : net.c
	$(GCC) -c net.c

last : $(objects)
	$(GCC) -o last $(objects)
	$(STP) last

clean :
	rm -rf last $(objects)
