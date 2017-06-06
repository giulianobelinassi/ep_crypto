##CC   = /usr/local/djgpp/bin/i586-pc-msdosdjgpp-gcc -march=i386# MS-DOS compiler
#CC    = i686-w64-mingw32-gcc-win32 -march=i486# Win32 compiler
CC     = gcc
CFLAGS = -g -O3 -flto
CEXTRA = -Wall
RM     = rm -rf

all: main

main: main.o k128.o key.o fileio.o endian.o metrics.o
	$(CC) $(CEXTRA) $(CFLAGS) -o $@ $^

main.o: main.c fileio.h k128.h
	$(CC) $(CEXTRA) $(CFLAGS) -c $<

fileio.o: fileio.c fileio.h
	$(CC) $(CEXTRA) $(CFLAGS) -c $<

key.o: key.c endian.o key.h endian.h
	$(CC) $(CEXTRA) $(CFLAGS) -c $<

endian.o: endian.c endian.h
	$(CC) $(CEXTRA) $(CFLAGS) -c $<

k128.o: k128.c key.o endian.o endian.h
	$(CC) $(CEXTRA) $(CFLAGS) -c $<

metrics.o: metrics.c k128.o key.o endian.o 
	$(CC) $(CEXTRA) $(CFLAGS) -c $<

clean:
	rm -f *.o main test *.cry *.dec

test_fileio: fileio.c
	@$(CC) $(CFLAGS) -o test $^ -DTEST_FILEIO
	@./test
	@$(RM) test *.test

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

test_metrics: metrics.c endian.c key.c k128.c
	@$(CC) $(CFLAGS) -o test $^ -DTEST_METRICS
	@./test
	@$(RM) test

test_acceptance: main
	@echo "File after encryption-decryption process should be the same as the original."
	@./main -c -i main.c -o main.cry -p cripto123
	@./main -d -i main.cry -o main.dec -p cripto123
	@cmp main.dec main.c
	@echo "OK"

tests: test_fileio test_key test_endian test_k128 test_metrics test_acceptance
	
