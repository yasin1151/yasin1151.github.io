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

A* newFunc(size_t tag)
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
}

class newFunClass
{
public:
	A* operator() (size_t tag)
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
	}
};

int main()
{
	SimpleMemPool<A> memPool;

	//memPool.setNewFunc(newFunClass());
	//memPool.setNewFunc(newFunc);

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