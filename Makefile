CC=aarch64-linux-gnu-g++
CFLAGS=-fPIC -Wall
LDFLAGS=-shared
TARGET_LIB=libtcppi.so

all: $(TARGET_LIB)

$(TARGET_LIB): tcppi.o
	$(CC) ${LDFLAGS} -o $@ $^

tcppi.o: tcppi.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.so

.PHONY: all clean
