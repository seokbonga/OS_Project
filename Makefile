hw1	:	fs.o	disk.o	testcase.o	validate.o
	gcc	-no-pie	-o	hw1	fs.o	disk.o	testcase.o	validate.o
fs.o	:	fs.c	disk.h
	gcc	-c	fs.c
disk.o	:	disk.c	disk.h
	gcc	-c	disk.c
testcase.o	:	testcase.c	fs.h	disk.h	validate.o
	gcc	-c	testcase.c
clean:	
	rm	fs.o	disk.o	testcase.o	hw1
