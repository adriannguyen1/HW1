#makefile!!! I used the same format as the ones we had in Prof Butner's ECS 50 class

OPTIONS := -Wall -Wextra -Werror

sshell: sshell.o
	gcc $(OPTIONS) -o sshell sshell.o

sshell.o: sshell.c
	gcc $(OPTIONS) -g -c -o sshell.o sshell.c

#Clean command!
clean:
	rm *.o sshell