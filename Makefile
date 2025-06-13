CC=aarch64-linux-gnu-g++-9
CFLAGS=-fPIC -Wall -pthread
LDFLAGS=-shared -L. -lARMLIB -pthread
TARGET_LIB=libtcppi.so

all: $(TARGET_LIB)

$(TARGET_LIB): tcppi.o
	$(CC) ${LDFLAGS} -o $@ $^

tcppi.o: tcppi.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o libtcppi.so

.PHONY: all clean
