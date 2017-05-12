/****************************************************************************

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

****************************************************************************/

#ifndef _SimpleMemPool_H_
#define _SimpleMemPool_H_

#include <list>
#include <functional>
#include <algorithm>

/**
*	@name	: SimpleMemPool.h
*	@auther : PengYao
*	@version: 1.0
*	@des	: a simple mempool, to manager pointer
*/


template <typename T>
class SimpleMemPool
{
protected:

	typedef int size_t;
	typedef std::function<T*(size_t tag)> NewFunc;

	/**
	*	@class	 : MemTagNode
	*	@brief   : to save pointer and tag
	*	@warnning: default tag is 0
	*/
	class MemTagNode
	{
	public:
		T* _pointer;
		size_t _tag;

		MemTagNode(T* pointer, size_t tag = 0)
			:_pointer(pointer),
			_tag(tag)
		{}
	};

protected:
	//unused memory
	std::list<MemTagNode> m_listUnUsedMem;

	//used memory
	std::list<MemTagNode> m_listUsedMem;

	//user-defined new function
	NewFunc m_pNewFunc;

	//when _listUnUsedMem can't find Mem, Invoke NewFunc times
	size_t m_iInvokeFuncTimes;

	/**
	*	@brief : to find a node with same tag
	*	@param _list : need to find list
	*	@param tag : tag
	*	@return : if find return iterator else return _list.end()
	*/
	typename std::list<MemTagNode>::iterator findWithTag(std::list<MemTagNode>& _list, size_t tag);

public:

	//default constructer
	SimpleMemPool()
		:m_pNewFunc(nullptr)
		, m_iInvokeFuncTimes(1)
	{}


	//NewFunc
	SimpleMemPool(const NewFunc& newFunc, size_t invokeFuncTimes = 5)
		:m_pNewFunc(newFunc)
		, m_iInvokeFuncTimes(invokeFuncTimes)
	{}

	//default destructer
	~SimpleMemPool(){}

	/**
	*	@brief : alloc a memory with same tag
	*	@param tag : user-defined tag
	*	@return : if there is not mush memory, will call NewFunc, if failed, return nullptr
	*	else return user required memory
	*/
	T* allocWithTag(size_t tag);

	/**
	*	@brief : recycle the already allocated mem
	*	@param mem : already allocated mem
	*	@return : if the mem in _listUsedMem, return true, else return flase
	*/
	bool release(T* mem);

	/**
	*	@brief : clear up the _listUsedMem and _listUnUsedMem
	*/
	void clear();

	/**
	*	@brief : set the simple factory pattern func
	*	@param newFunc : func origin-type : T*(*NewFunc)(size_t tag)
	*/
	void setNewFunc(const NewFunc& newFunc);

	/**
	*	@brief : set the times when _listUnUsed can't find need mem
	*	@param times : Invoke times
	*/
	void setCallNum(int times)
	{
		m_iInvokeFuncTimes = times;
	}

	/**
	*	@brief : user manual operator NewFunc
	*	@param tag : new mem tag
	*	@param callNum : invoke num
	*/
	void callNewFunc(size_t tag, size_t callNum);
};

template <typename T>
typename std::list<typename SimpleMemPool<T>::MemTagNode>::iterator SimpleMemPool<T>::findWithTag(std::list<MemTagNode>& _list, size_t tag)
{
	return find_if(_list.begin(), _list.end(), 
		[&](const MemTagNode& tagNode)
	{
		return tagNode._tag == tag;
	});
}


template <typename T>
T* SimpleMemPool<T>::allocWithTag(size_t tag)
{
	T* pRet = nullptr;

	//to traverse the list
	auto it = this->findWithTag(m_listUnUsedMem, tag);

	if (it == m_listUnUsedMem.end() || m_listUnUsedMem.empty())
	{
		//try to call NewFunc
		if (m_pNewFunc)
		{
			//invoke NewFunc
			this->callNewFunc(tag, m_iInvokeFuncTimes);

			//find the can use mem
			auto itUnUsed = this->findWithTag(m_listUnUsedMem, tag);

			//if success to invoke NewFunc
			if (itUnUsed != m_listUnUsedMem.end())
			{
				//to get the pointer
				pRet = (*itUnUsed)._pointer;

				//append to _listUsedMem
				m_listUsedMem.push_back(*itUnUsed);

				//erase from _listUnUsedMem
				m_listUnUsedMem.erase(itUnUsed);
			}
		}
	}
	else
	{
		//assign the pRet
		pRet = (*it)._pointer;

		//to append on _listUsedMem
		m_listUsedMem.push_back(*it);

		//earse from _listUnUsedMem
		m_listUnUsedMem.erase(it);
	}

	return pRet;
}

template <typename T>
bool SimpleMemPool<T>::release(T* mem)
{
	if (!mem)
	{
		return false;
	}

	//to find same pointer node
	auto it = find_if(m_listUsedMem.begin(), m_listUsedMem.end(),
		[&](const MemTagNode& tagNode)
	{
		return tagNode._pointer == mem;
	});

	if (it != m_listUsedMem.end())
	{
		//append to _listUnUseddMem
		m_listUnUsedMem.push_back(*it);

		//erase from _listUsedMem
		m_listUsedMem.erase(it);

		return true;
	}

	return false;
}

template <typename T>
void SimpleMemPool<T>::clear()
{
	//clear up the _listUnUsedMem
	m_listUnUsedMem.clear();

	//clear up the _listUnedMem
	m_listUsedMem.clear();

	//set m_pNewFunc empty
	m_pNewFunc = nullptr;
}

template <typename T>
void SimpleMemPool<T>::setNewFunc(const NewFunc& newFunc)
{
	m_pNewFunc = newFunc;
}

template <typename T>
void SimpleMemPool<T>::callNewFunc(size_t tag, size_t callNum)
{
	T* pBuf = nullptr;
	for (int i = 0; i < callNum; i++)
	{
		pBuf = m_pNewFunc(tag);
		if (!pBuf)
		{
			//run out of memory
			break;
		}

		//save time to find new Element
		m_listUnUsedMem.push_front(MemTagNode(pBuf, tag));

		//set pointer empty
		pBuf = nullptr;
	}
}



#endif
