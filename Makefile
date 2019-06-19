all:
	gcc -o ring_buffer_test ring_buffer_test.c  ring_buffer.c -I./ -lpthread
	
clean:
	rm -rf ring_buffer_test
