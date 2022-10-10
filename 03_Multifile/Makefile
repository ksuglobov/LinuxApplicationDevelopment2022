# files we have
GENERATES = prog prog-a prog-so liboutput-static.a liboutput.so
TRASH = *.o *~ o.*
OUTPUTS = *.$(OUT_FORMAT)

# compilation settings
CFLAGS = -fPIC
LD_PATH = LD_LIBRARY_PATH=`pwd`
OUT_FORMAT = output
STDERR_REDIRECT = /dev/null

# args cases
ARGS_1 = ""
ARGS_2 = "lorem"
ARGS_3 = "lorem ipsum dolor"


all:	prog prog-a prog-so

prog:	const.o fun.o prog.o

prog-a:	prog.o liboutput-static.a
	cc -L. $< -loutput-static -o $@

prog-so:	prog.o liboutput.so
	cc -L. $< -loutput -o $@

liboutput-static.a:	fun.o const.o
	ar -rcs $@ $^

liboutput.so:	fun.o const.o
	cc -shared $^ -o $@

fun.o: outlib.h

prog.o: outlib.h

test: test-1 test-2 test-3

test-%:
	$(LD_PATH) ./prog ${ARGS_$*} > prog_$*.${OUT_FORMAT} 2> ${STDERR_REDIRECT}
	$(LD_PATH) ./prog-a ${ARGS_$*} > prog-a_$*.${OUT_FORMAT} 2> ${STDERR_REDIRECT}
	$(LD_PATH) ./prog-so ${ARGS_$*} > prog-so_$*.${OUT_FORMAT} 2> ${STDERR_REDIRECT}
	cmp prog_$*.${OUT_FORMAT} prog-a_$*.${OUT_FORMAT}
	cmp prog_$*.${OUT_FORMAT} prog-so_$*.${OUT_FORMAT}

clean:
	rm -f $(TRASH) $(OUTPUTS)

distclean:	clean
	rm -rf $(GENERATES)