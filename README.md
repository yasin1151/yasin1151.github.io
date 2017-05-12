# SimpleMemPool

> auther : PengYao  
time : 2017/5/12    
version : 2.0

一个简单的内存池，只存储对象指针    
耦合了一个简单工厂模式(支持lambda，函数对象，普通函数)，用于在内存不足时自动调用

example : 

```
#include <iostream>
#include "SimpleMemPool.h"
using namespace std;


class A
{
public:
	int a;
	int b;

	A(int a_, int b_)
		:a(a_), b(b_)
	{

	}
};

class B : public A
{
public:
	int c;
	B(int a_, int b_, int c_)
		:A(a_, b_), c(c_)
	{
		
	}
};

int main()
{
	SimpleMemPool<A> memPool;

	memPool.setNewFunc([](size_t tag)
	{
		A* pRet = nullptr;
		switch (tag)
		{
		case 1:
			pRet = new A(1, 2);
			break;
		case 2:
			pRet = new B(3, 4, 5);
			break;
		}
		return pRet;
	});

	memPool.setCallNum(5);

	A* demoA = memPool.allocWithTag(1);
	cout << "demoA : "<< demoA << " "<< demoA->a << " " << demoA->b << endl;

	B* demoB = reinterpret_cast<B*>(memPool.allocWithTag(2));
	cout << "demoB : " << demoB << " " << demoB->a << " " << demoB->b << " " << demoB->c << endl;

	memPool.release(demoA);
	memPool.release(demoB);

	A* demoC = memPool.allocWithTag(1);
	cout << "demoC : " << demoC << " " << demoC->a << " " << demoC->b << endl;

	B* demoD = reinterpret_cast<B*>(memPool.allocWithTag(2));
	cout << "demoD : " << demoD << " " << demoD->a << " " << demoD->b << " " << demoD->c << endl;


	return 0;
}
```


output :

```
demoA : 017B02D0 1 2
demoB : 017B01B8 3 4 5
demoC : 017B03B0 1 2
demoD : 017B0490 3 4 5
```
