# algorithm
good algorithm from others or myself

# 算法
timer/timewheel/itimer 来源于https://github.com/skywind3000/AsyncNet/blob/master/system/ 将该算法记录下来，以备用学习


itimer_pool 是修改itimer，增加了一个内存池管理和简化设置和去掉定时器的操作


itimer_pool_thread 是修改itimer为一个独立线程来管理定时器，通过2个简单的宏和一个函数就可以嵌入到其他线程去处理定时器

min_heap 是最小堆实现的定时器，这个跟epoll_wait等IO多路复用函数一起使用比较好

# 框架
zebra thread 是开源软件zebra剥离出来的运行框架，用在单线程的程序里面效果很好



# 工具
tools/dead_lock_stub 是我自己写的一个测试死锁的代码，是根据网上大神说的通过图的算法来测试死锁的，可以检查到死锁和锁等待。已经在正式环境中使用过了。具体使用在内部的README.txt中说明





# 设计模式
design_patterns
这是我自己写的设计模式的简单demo和一些笔记。主要参考着《大话设计模式》来编写的，谢谢作者。