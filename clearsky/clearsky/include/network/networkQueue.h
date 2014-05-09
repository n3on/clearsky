#ifndef _NETWORK_QUEUE_H_
#define _NETWORK_QUEUE_H_

#include "network\networkIncludes.h"
#include "network\networkData.h"

#include <queue>

namespace clearsky
{

	/*
	 * For a thread safe version, use
	 * NetworkQueueTS below
	 *********************************/

	typedef NetworkDataMessage queueData;

	class CLEARSKY_API NetworkQueue
	{
	public:

		NetworkQueue();
		virtual ~NetworkQueue();

		// Clear the queue
		virtual const RETURN_VALUE clear();

		// Insert new data at the end
		virtual const RETURN_VALUE pushBack(const queueData& netData);

		// Determine number of outstanding requests
		const unsigned int getCount() const;

		const bool isEmpty() const;

		// Retrieve queued data
		virtual const queueData getOldest(const bool remove = true);

		virtual const queueData getLatest() const;
		
	protected:
		std::queue<queueData> m_Queue;
		const RETURN_VALUE doClear();
		const RETURN_VALUE doPushBack(const queueData& netData);
		const unsigned int doGetCount() const;
		const bool doIsEmpty() const;
		const queueData doGetOldest(const bool remove = true);
		const queueData doGetLatest() const;
		
	};


	//Thread Safe NetworkQueue

	class CLEARSKY_API NetworkQueueTS : public NetworkQueue
	{
	public:
		 NetworkQueueTS();
		 ~NetworkQueueTS();

		 // Clear the queue
		virtual const RETURN_VALUE clear();

		// Insert new data at the end
		virtual const RETURN_VALUE pushBack(const queueData& netData);

		// Retrieve queued data
		virtual const queueData getOldest(const bool remove = true);

		virtual const queueData getLatest() const;

	private:
		mutable CRITICAL_SECTION m_critSec;
	};
}

#endif