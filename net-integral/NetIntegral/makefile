CFLAGS=  -g -Wall -pthread -std=c99

calc: calculator.o sys_info.o
	$(CC) $(CFLAGS)   $^ -o $@

distr: distributor.o
	$(CC) $(CFLAGS)   $^ -o $@

clean:
	rm -f *.o *.d *.exe *.gcov *.gcda *.gcno calc distr
