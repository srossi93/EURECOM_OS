all :
	gcc -Wall -o mq01 mq01.c -lrt
	gcc -Wall -o mq02 mq02.c -lrt
	gcc -Wall -o mq03 mq03.c -lrt
run:
	./mq03
trace:
	dtrace -c mqtest -n 'pid$target:::entry'
