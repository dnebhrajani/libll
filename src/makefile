
lltst_int: libll.o lltst_int.o
	gcc -Wall libll.o lltst_int.o -o lltst_int

lltst_float: libll.o lltst_float.o
	gcc -Wall libll.o lltst_float.o -o lltst_float

libll.o: ../src/libll.c ../src/libll.h
	gcc -Wall -c ../src/libll.c

lltst_int.o: ../test/lltst_int.c ../src/libll.h
	gcc -Wall -I../src -c ../test/lltst_int.c

lltst_float.o: ../test/lltst_float.c ../src/libll.h
	gcc -Wall -I../src -c ../test/lltst_float.c

clean:
	\rm -f lltst libll.o lltst.o

clobber:
	\rm -f lltst *.o *~ a.out
