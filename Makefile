jack_interposer.so: jack_interposer.c
	gcc -o jack_interposer.so -shared jack_interposer.c -ljack
