# Make Proj08
# Tianrui Liu

proj08: proj08.student.o
	g++ -std=c++11 -Wall proj08.student.o -o proj08
proj08.student.o: proj08.student.c
	g++ -std=c++11 -Wall -c proj08.student.c
