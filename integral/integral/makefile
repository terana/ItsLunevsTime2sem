CFLAGS=  -g -Wall -pthread -std=c99

SOURCES=integral.c sys_info.c
TARGET=integral

$(TARGET): $(SOURCES:.c=.o)
	$(CC) $(CFLAGS)   $^ -o $@

test: test.o sys_info.o
	$(CC) $(CFLAGS)   $^ -o $@

clean:
	rm -f *.o *.d *.exe *.gcov *.gcda *.gcno
