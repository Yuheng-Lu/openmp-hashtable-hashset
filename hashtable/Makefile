CC = gcc
CFLAGS = -O2 -fopenmp

# Define key type
KEY_T ?= uint32_t

ifeq ($(KEY_T),uint32_t)
CFLAGS += -DKEY_T=uint32_t -DK_EMPTY=0xFFFFFFFFU
else ifeq ($(KEY_T),char)
CFLAGS += -DKEY_T=char -DK_EMPTY=-1
else
$(error Unsupported KEY_T value)
endif

# Target
TARGET = benchmark

all: $(TARGET)

$(TARGET): benchmark.o hashtable.o
	$(CC) $(CFLAGS) -o $@ $^

benchmark.o: benchmark.c hashtable.h
	$(CC) $(CFLAGS) -c $< -o $@

hashtable.o: hashtable.c hashtable.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)
