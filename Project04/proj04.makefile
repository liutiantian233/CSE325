# Make Proj04
# Tianrui Liu

proj04: proj04.student.o
	g++ -std=c++11 -Wall -o proj04 proj04.student.o
proj04.student.o: proj04.student.c
	g++ -std=c++11 -Wall -c proj04.student.c
