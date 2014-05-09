#ifndef _NETWORK_SOCKET_H_
#define _NETWORK_SOCKET_H_

#include "network\networkIncludes.h"
#include "network\networkData.h"
#include "network\networkQueue.h"


namespace clearsky
{
	typedef std::map<CLIENT_UID, SOCKADDR_IN> CLIENT_INFO;

	class CLEARSKY_API NetworkManager
	{
	public:

		NetworkManager();
		~NetworkManager();

		const bool isInitialized() const;

		// Returns 0 or main version:
		const unsigned short getVersion() const;

	private:

		NetworkManager(const NetworkManager&);

		WSADATA m_wsa;
		bool m_initialized;
	};

	class CLEARSKY_API Socket
	{
	public:

		Socket(const NetworkManager& netMan);
		virtual ~Socket();
		const bool isInitialized() const;

		virtual const bool sendNDM(const NetworkDataMessage&) = 0;
		virtual void fillQueue(NetworkQueueTS& nq) = 0;
		virtual bool stopQueuing() = 0;

	protected:

		SOCKET m_sock;

	private:

		bool m_initialized;
	};

	class CLEARSKY_API ClientSocket : public Socket
	{
	public:

		ClientSocket(const NetworkManager& netMan);
		virtual ~ClientSocket();

		void fillQueue(NetworkQueueTS& nq);
		bool stopQueuing();
		bool initiate(const unsigned short port, const unsigned long addr);

		const bool sendNDM(const NetworkDataMessage& ndm);

	private:
		SOCKADDR_IN address;
		CLIENT_UID m_uid;
	};

	class CLEARSKY_API ServerSocket : public Socket
	{
	public:

		ServerSocket(const NetworkManager& netMan, const unsigned short port, const unsigned long addr = ADDR_ANY);
		virtual ~ServerSocket();

		const bool isListening() const;

		void fillQueue(NetworkQueueTS& nq);
		bool stopQueuing();

		const bool sendNDM(const NetworkDataMessage& ndm);

	private:

		bool m_isListening;
		bool m_stop;

		CLIENT_INFO m_clients;
		CLIENT_INFO::iterator m_iter;
		CLIENT_UID m_idCounter;

		CRITICAL_SECTION m_critSec;
	};
}

#endif