lltst: libll.o lltst.o
	gcc -Wall libll.o lltst.o -o lltst
libll.o: ../src/libll.c ../src/libll.h
	gcc -Wall -c ../src/libll.c
lltst.o: ../test/lltst.c ../src/libll.h
	gcc -Wall -I../src -c ../test/lltst.c
clean:
	\rm -f lltst libll.o lltst.o
clobber:
	\rm -f lltst *.o *~ a.out
