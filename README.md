功能描述：
ring buffer测试程序，创建两个线程，一个生产者，一个消费者。
生产者每隔1秒向buffer中投入数据，消费者每隔2秒去取数据。


编译方法：
gcc -o ring_buffer_test ring_buffer_test.c  ring_buffer.c -I./ -lpthread


上传本地代码到GitHub上：
1. 在GitHub上新建一个项目，取一个Repository name，不能和其他项目冲突
2. 在本地一个文件夹内，使用 git clone https://github.com/tcpaidaxing/ring_buffer.git命令将上面的仓库克隆到本地，本地文件夹内会生成一个ring_buffer文件夹
3. 将所要上传的代码拷贝到ring_buffer文件夹内
4. git add .
5. git commit -m "first commit"     (引号内的内容可以自定义)
6. git push -u origin master        (此操作的目的是将本地项目push到GitHub上去)
