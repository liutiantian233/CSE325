# Make Proj10
# Tianrui Liu

proj10: proj10.student.o
	g++ -std=c++11 -Wall proj10.student.o -o proj10
proj10.student.o: proj10.student.c
	g++ -std=c++11 -Wall -c proj10.student.c