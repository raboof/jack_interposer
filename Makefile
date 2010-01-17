jack_interposer.so: jack_interposer.c
	gcc -Wall -o jack_interposer.so -shared jack_interposer.c -pthread

.PHONY clean: 
	rm jack_interposer.so test_cond_wait test_cond_wait_simple 

test: test_new test_cond_wait test_cond_wait_simple jack_interposer.so
	LD_PRELOAD=./jack_interposer.so ./test_cond_wait_simple
	LD_PRELOAD=./jack_interposer.so ./test_cond_wait
	LD_PRELOAD=./jack_interposer.so ./test_new

test_new: test_new.cpp
	g++ -o test_new test_new.cpp -ljack

test_cond_wait_simple: test_cond_wait_simple.c
	gcc -o test_cond_wait_simple test_cond_wait_simple.c -pthread -ldl

test_cond_wait: test_cond_wait.c
	gcc -o test_cond_wait test_cond_wait.c -ljack -lpthread
