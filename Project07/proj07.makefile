# Make Proj07
# Tianrui Liu

proj07: proj07.student.o
	g++ -std=c++11 -Wall proj07.student.o -o proj07
proj07.student.o: proj07.student.c
	g++ -std=c++11 -Wall -c proj07.student.c
