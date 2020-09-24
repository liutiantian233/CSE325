# Make Proj09
# Tianrui Liu

proj09: proj09.student.o
	g++ -std=c++11 -Wall proj09.student.o -o proj09
proj09.student.o: proj09.student.c
	g++ -std=c++11 -Wall -c proj09.student.c
