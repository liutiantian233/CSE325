proj03: proj03.student.o
	gcc -Wall proj03.student.o -o proj03

proj03.student.o: proj03.student.c
	gcc -Wall -c proj03.student.c
