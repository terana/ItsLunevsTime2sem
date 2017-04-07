CFLAGS=  -g -Wall -pthread -std=c99

SOURCES=integral.c
TARGET=integral

$(TARGET): $(SOURCES:.c=.o)
	$(CC) $(CFLAGS)   $^ -o $@

clean:
	rm -f *.o *.d *.exe *.gcov *.gcda *.gcno
