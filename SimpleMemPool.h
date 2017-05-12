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
public:

	//default constructer
	SimpleMemPool()
		:m_pNewFunc(nullptr)
	{}

	//NewFunc
	SimpleMemPool(const NewFunc& newFunc)
		:m_pNewFunc(newFunc)
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
};

template <typename T>
T* SimpleMemPool<T>::allocWithTag(size_t tag)
{
	T* pRet = nullptr;

	//to traverse the list
	auto it = find_if(m_listUnUsedMem.begin(), m_listUnUsedMem.end(),
		[&](const MemTagNode& tagNode)
	{
		return tagNode._tag == tag;
	});

	if (it == m_listUnUsedMem.end() || m_listUnUsedMem.empty())
	{
		//try to call NewFunc
		if (m_pNewFunc)
		{
			pRet = m_pNewFunc(tag);

			//call success
			if (pRet)
			{
				m_listUsedMem.push_back(MemTagNode(pRet, tag));
				return pRet;
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


#endif