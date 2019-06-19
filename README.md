功能描述：
ring buffer测试程序，创建两个线程，一个生产者，一个消费者。
生产者每隔1秒向buffer中投入数据，消费者每隔2秒去取数据。


编译方法：
gcc -o ring_buffer_test ring_buffer_test.c  ring_buffer.c -I./ -lpthread
