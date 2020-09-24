# Make Proj06
# Tianrui Liu

proj06: proj06.student.o
	g++ -std=c++14 -Wall -lpthread -o proj06 proj06.student.o
proj06.student.o: proj06.student.c
	g++ -std=c++14 -Wall -c proj06.student.c
