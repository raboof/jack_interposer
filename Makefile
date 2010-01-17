jack_interposer.so: jack_interposer.c
	gcc -o jack_interposer.so -shared jack_interposer.c 

test: test_cond_wait test_cond_wait_simple jack_interposer.so
	LD_PRELOAD=./jack_interposer.so ./test_cond_wait_simple

test_cond_wait_simple: test_cond_wait_simple.c
	gcc -o test_cond_wait_simple test_cond_wait_simple.c -lpthread -ldl

test_cond_wait: test_cond_wait.c
	gcc -o test_cond_wait test_cond_wait.c -ljack -lpthread
