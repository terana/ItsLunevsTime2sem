CFLAGS=  -g -Wall -fprofile-arcs -ftest-coverage
CC=clang

SOURCES= list.c list_test.c
TARGET=test

$(TARGET): $(SOURCES:.c=.o)
	$(CC) $(CFLAGS)   $^ -o $@

clean:
	rm -f *.o *.d *.exe *.gcov *.gcda *.gcno