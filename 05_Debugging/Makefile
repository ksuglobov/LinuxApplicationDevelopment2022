GENERATES=range gdb*.out *.tmp *.txt

all: build

build:
	gcc range.c -g -O0 -Wall -Wextra -o range

gdb_script_%.out: build
	gdb -q -batch -x gdb_script_$* --args ./range > /dev/null

test_%: gdb_script_%.out
	cat $< | grep "@@@" | tr -d '@' > $<.tmp
	cmp $<.tmp test_$*_ans.sample

test: test_1 test_2

clean:
	rm -rf $(GENERATES)