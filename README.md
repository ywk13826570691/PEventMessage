# PEventMessage
Processes using udp to Subscibe and Observer Event， Support many process to communicate through Events
1，介绍
     此代码实现在linux平台下，多进程之间实现观察者和订阅者模式，多进程之间可以发布事件，只要订阅此事件的进程都能收到此事件， 代码用纯C++编写，不依赖其他第三方库，使用udp通信
2，使用：
   1，make
   2，在目录下生成dispatch_server和client1...client3
   3，分别运行这三个应用软件，client3发布的EventTest将会被client1和client2接受，目前只实现了一个事件模型EventTest，其他事件模型可以参考这个模型实现
   
3，详细介绍：
  client目录下是客户端模型，用户只要继承此模型类然后实现自己需要关心的事件虚函数，和订阅自己关心的事件即OK，用户可以参考TestClient1.cpp ...TestClient3.cpp
  server目录主要是server的实现，用户不需要使用里边代码，直接编译成server运行在后台即使OK
  event目录是事件模型，用户可以根据自身的需求定义各种事件在此目录下，参考EventTest模型实现OK
  include目录包含一些公共头文件
  
4,如何添加自定义事件模型
  1，自定义一个事件类继承Event类，并在EventId.h文件中给此事件定义一个ID，每个事件有唯一的ID
  2，参考EventTest实现虚函数
  3，在EventHandle类中添加事件虚拟处理函数，这个函数是个空函数，EventHandle是提供客户端类来继承
  4，在GlobalEvents.cpp文件中new一个事件就OK
  5，这样每个客户端就可以自由发布订阅此事件
  
5，注意
  此系统不支持通过事件传输大数据最大4K，比如要传递10M数据，如果想传递10M数据，那么用户需要自己定义一个事件，通过事件协议分多次发送数据，或者通过文件方式传递
  比如传输10M数据，用户定义一个事件，这个事件在传输10M数据之前先发送一个开始标志位在事件模型中，然后每个事件发送1K数据，最后一个事件时发送一个结束标记在事件模型中，这样就能传输10M数据，或者用户将数据保存在文件中然后发送文件名字过去即OK，我们UDP和TCP网络协议本来是不支持大数据传输的，所以我们不建议传输大数据一次
  
  

