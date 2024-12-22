CC = gcc
CFLAGS = -O3 -fopenmp

# Define key type
KEY_T ?= uint32_t

ifeq ($(KEY_T),uint32_t)
CFLAGS += -DKEY_T=uint32_t -DK_EMPTY_SET=0xFFFFFFFFU
else ifeq ($(KEY_T),char)
CFLAGS += -DKEY_T=char -DK_EMPTY_SET=-1
else
$(error Unsupported KEY_T value)
endif

# Target
TARGET = benchmark

all: $(TARGET)

$(TARGET): benchmark.o hashset.o
	$(CC) $(CFLAGS) -o $@ $^

benchmark.o: benchmark.c hashset.h
	$(CC) $(CFLAGS) -c $< -o $@

hashset.o: hashset.c hashset.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)
