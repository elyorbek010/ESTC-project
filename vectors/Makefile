program: vector.o
	gcc -o program vector.o

main.o: vector.c add_vec.h sub_vec.h dot_vec.h cross_vec.h
	gcc -c vector.c

clean:
	rm -f vector.o program

.PHONY: clean
