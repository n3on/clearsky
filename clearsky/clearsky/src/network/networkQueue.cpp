#include "core/precompiled.h"
#include "network/networkQueue.h"

namespace clearsky
{

	NetworkQueue::NetworkQueue()
	{
	}

	NetworkQueue::~NetworkQueue()
	{
	}


	const RETURN_VALUE NetworkQueue::clear()
	{
		
		return doClear();
	}

	const RETURN_VALUE NetworkQueue::doClear()
	{
		
		while (!this->m_Queue.empty())
		{
			this->m_Queue.pop();
		}
		return RETURN_OK;
	}


	const RETURN_VALUE NetworkQueue::doPushBack(const queueData& netData)
	{
		this->m_Queue.push(netData);
		return RETURN_OK;
	}

	const RETURN_VALUE NetworkQueue::pushBack(const queueData& netData)
	{
		doPushBack(netData);
		return RETURN_OK;
	}


	const unsigned int NetworkQueue::getCount() const
	{
		return doGetCount();
	}

	const unsigned int NetworkQueue::doGetCount() const
	{
		return (unsigned int)this->m_Queue.size();
	}

	const bool NetworkQueue::doIsEmpty() const
	{
		return this->m_Queue.empty();
	}

	const bool NetworkQueue::isEmpty() const
	{
		return doIsEmpty();
	}

	const queueData NetworkQueue::doGetOldest(const bool remove)
	{
		queueData ret = this->m_Queue.front();
		if (remove)
		{
			this->m_Queue.pop();
		}
		return ret;
	}

	const queueData NetworkQueue::getOldest(const bool remove)
	{
		return this->doGetOldest(remove);
	}

	const queueData NetworkQueue::doGetLatest()	const
	{
		return this->m_Queue.back();
	}

	const queueData NetworkQueue::getLatest() const
	{
		return this->doGetLatest();
	}



	NetworkQueueTS::NetworkQueueTS():NetworkQueue()
	{
		InitializeCriticalSection(&this->m_critSec);
	}

	NetworkQueueTS::~NetworkQueueTS()
	{
		DeleteCriticalSection(&this->m_critSec);
	}

	const RETURN_VALUE NetworkQueueTS::clear()
	{
		RETURN_VALUE ret;
		EnterCriticalSection(&this->m_critSec);
		{
			ret = doClear();
		}
		LeaveCriticalSection(&this->m_critSec);
		return ret;
	}

	const RETURN_VALUE NetworkQueueTS::pushBack(const queueData& netData)
	{
		RETURN_VALUE ret = RETURN_OK;	

		EnterCriticalSection(&this->m_critSec);
		{
			doPushBack(netData);
		}
		LeaveCriticalSection(&this->m_critSec);

		return ret;
	}

	const queueData NetworkQueueTS::getOldest(const bool remove)
	{
		queueData ret;

		EnterCriticalSection(&this->m_critSec);
		{
			ret = this->doGetOldest(remove);
		}
		LeaveCriticalSection(&this->m_critSec);

		return ret;
	}

	const queueData NetworkQueueTS::getLatest()	const
	{
		queueData ret;

		EnterCriticalSection(&this->m_critSec);
		{
			ret = this->doGetLatest();
		}
		LeaveCriticalSection(&this->m_critSec);

		return ret;
	}

}