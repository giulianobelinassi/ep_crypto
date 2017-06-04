CC     = gcc
CFLAGS = -std=c89 -g #-O3 -flto
CEXTRA = -Wall
RM     = rm -rf

all: main

main: main.o k128.o key.o fileio.o endian.o
	$(CC) $(EXTRA) $(CFLAGS) -o $@ $^

main.o: main.c fileio.h k128.h
	$(CC) $(EXTRA) $(CFLAGS) -c $<

fileio.o: fileio.c fileio.h
	$(CC) $(EXTRA) $(CFLAGS) -c $<

key.o: key.c endian.o key.h endian.h
	$(CC) $(EXTRA) $(CFLAGS) -c $<

endian.o: endian.c endian.h
	$(CC) $(EXTRA) $(CFLAGS) -c $<

k128.o: k128.c key.o endian.o endian.h
	$(CC) $(EXTRA) $(CFLAGS) -c $<

clean:
	rm -f *.o main test

test_fileio: fileio.c
	@$(CC) $(CFLAGS) -o test $^ -DTEST_FILEIO
	@./test
	@$(RM) test

test_key: key.c endian.c
	@$(CC) $(CFLAGS) -o test $^ -DTEST_KEY
	@./test
	@$(RM) test

test_endian: endian.c
	@$(CC) $(CFLAGS) -o test $^ -DTEST_ENDIAN
	@./test
	@$(RM) test

test_k128: k128.c endian.c key.c
	@$(CC) $(CFLAGS) -o test $^ -DTEST_K128
	@./test
	@$(RM) test

tests: test_fileio test_key test_endian test_k128
	
