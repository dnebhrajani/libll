
lltst_string: libll.o lltst_string.o
	gcc -Wall libll.o lltst_string.o -o lltst_string

lltst_char: libll.o lltst_char.o
	gcc -Wall libll.o lltst_char.o -o lltst_char

lltst_int: libll.o lltst_int.o
	gcc -Wall libll.o lltst_int.o -o lltst_int

lltst_float: libll.o lltst_float.o
	gcc -Wall libll.o lltst_float.o -o lltst_float

libll.o: ../src/libll.c ../src/libll.h
	gcc -Wall -c ../src/libll.c

lltst_string.o: ../test/lltst_string.c ../src/libll.h
	gcc -Wall -I../src -c ../test/lltst_string.c

lltst_char.o: ../test/lltst_char.c ../src/libll.h
	gcc -Wall -I../src -c ../test/lltst_char.c

lltst_int.o: ../test/lltst_int.c ../src/libll.h
	gcc -Wall -I../src -c ../test/lltst_int.c

lltst_float.o: ../test/lltst_float.c ../src/libll.h
	gcc -Wall -I../src -c ../test/lltst_float.c

clean:
	\rm -f lltst libll.o lltst.o

clobber:
	\rm -f lltst *.o *~ a.out
