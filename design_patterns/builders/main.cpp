#include "builders.h"


/*
建造者模式
建造者模式和模板方法模式很类似，与模板方法模式的区别在于，将固定
的逻辑移动到一个叫做指挥者的类里面管理，由指挥者控制逻辑（流程）
，而指挥者封装一个抽象类指针，然后调用抽象类的虚函数来实际调用子
类对象的具体函数。耦合性比模板方法模式要低一点。但是会多一个控制
类出来。
建造者模式是在当创建复杂对象的算法应该独立于该对象的组成部分以及
它们的装配方式时适用的模式。
我的理解应该是单独弄出来一个控制类来确定对象的使用。然后控制类不
关心到底在调用哪个类对象，只要接口一样就行。

*/

//如果在cpp里面包含c定义的头文件
extern "C" {
}

int main(int argc, char **argv)
{
	personDirector p;
	boy a;
	girl b;
	p.do_build(a);
	p.do_build(b);

	return 0;
}

