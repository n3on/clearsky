#include "core/precompiled.h"
#include "network/networkSocket.h"

namespace clearsky
{

	/*
	*	Network Manager
	************************/

	NetworkManager::NetworkManager()
	{
		int result = WSAStartup(MAKEWORD(2,2), &m_wsa);

		if (result != 0)
		{
			m_initialized = false;
			LOG->logMsg(LT_ERROR, "WSAStartup failed with %d", result);
		}
		else
		{
			m_initialized = true;
		}

	}

	NetworkManager::~NetworkManager()
	{
		// This will happen only if there is no other instance of 
		// NetworkManager left! So it's multiple-instance-safe:
		WSACleanup();
	}

	const bool NetworkManager::isInitialized() const
	{
		return this->m_initialized;
	}


	const unsigned short NetworkManager::getVersion() const
	{
		if (this->m_initialized)
		{
			return this->m_wsa.wVersion;
		}
		else
		{
			return 0;
		}
	}

	/*
	*	Socket base class
	************************/

	Socket::Socket(const NetworkManager& netMan)
	{
		if (!netMan.isInitialized())
		{
			this->m_initialized = false;
			return;
		}
		
		this->m_sock = socket(AF_INET, SOCK_DGRAM, 0);

		if (this->m_sock == INVALID_SOCKET)
		{
			this->m_initialized = false;
			LOG->logMsg(LT_ERROR, "Couldn't create UDP socket.");
		}
	}

	Socket::~Socket()
	{
		closesocket(this->m_sock);
	}

	const bool Socket::isInitialized() const
	{
		return this->m_initialized;
	}


	/*
	*	Client Socket
	************************/

	ClientSocket::ClientSocket(const NetworkManager& netMan):Socket(netMan)
	{

	}

	ClientSocket::~ClientSocket()
	{

	}

	bool ClientSocket::initiate(const unsigned short port, const unsigned long addr)
	{
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = addr;
		address.sin_port = port;

		NetworkDataMessage initMsg;
		initMsg.NetData.MESSAGE_TYPE = MT_REQUEST_UID;
		initMsg.NetData.PROTOCOL = PROTOCOL_VERSION;
		initMsg.NetData.UID = UID_UNDEF;
		initMsg.NetData.MESSAGE_ARGS_LENGTH = 0;

		long rc = sendto(this->m_sock, initMsg.asChar, ND_SIZE, 0, (SOCKADDR*)&address, sizeof(SOCKADDR_IN));
		if (rc == SOCKET_ERROR)
		{
			LOG->logMsg(LT_ERROR, "Failed to send initialization request! Reason: %d", WSAGetLastError());
			return false;
		}
		
		int ral = 0;
		// Ungültiger Zeiger:
		rc = recvfrom(this->m_sock, initMsg.asChar, ND_SIZE, 0, (SOCKADDR*)&address, &ral);
		
		if (rc == SOCKET_ERROR)
		{
			LOG->logMsg(LT_ERROR, "Failed to receive initialization reply! Reason: %d", WSAGetLastError());
			return false;
		}

		if (initMsg.NetData.MESSAGE_TYPE == MT_REQUEST_UID)
		{
			this->m_uid = initMsg.NetData.UID;
			LOG->logMsg(LT_ERROR, "Acquired new Client UID: %d", this->m_uid);
			return true;
		}
		
		LOG->logMsg(LT_ERROR, "Wrong MESSAGE_TYPE received: %i", initMsg.NetData.MESSAGE_TYPE);
		return false;
	}
	void ClientSocket::fillQueue(NetworkQueueTS& nq)
	{
		/*
		SOCKADDR_IN remote_address;
		CLIENT_UID uid;
		int remoteAddrLen = sizeof(SOCKADDR_IN);
		unsigned long rc;
		NetworkDataMessage ndm;

		LOG->logMsg(LT_STATUS, "Yippie, Thread lives!");

		while (!this->m_stop)
		{
			rc = recvfrom(this->m_sock, ndm.asChar, ND_SIZE, 0, (SOCKADDR*)&remote_address, &remoteAddrLen);
			if (rc == SOCKET_ERROR)
			{
				LOG->logMsg(LT_ERROR, "recvfrom failed: %d", rc);
			}

			// Should be standard:
			else
			{
				// If this is the first packet from the client:
				if (ndm.NetData.UID == 0)
				{
					ndm.NetData.UID = this->m_idCounter;
					this->m_idCounter++;

					EnterCriticalSection(&this->m_critSec);
					this->m_clients.insert(std::make_pair(ndm.NetData.UID, remote_address));
					LeaveCriticalSection(&this->m_critSec);

					if (!this->sendNDM(ndm))
					{
						LOG->logMsg(LT_CRITICAL, "UID assignment failed!");
					}
				}
				else
				{
					nq.pushBack(ndm);
				}
			}
		}
		*/
	}

	const bool ClientSocket::sendNDM(const NetworkDataMessage& ndm)
	{
		/*
		bool ret = true;
		unsigned long rc;
		int remoteAddrLen = sizeof(SOCKADDR_IN);

		EnterCriticalSection(&this->m_critSec); //IN

		this->m_iter = this->m_clients.find(ndm.NetData.UID);
		// Should pracitically always be true:
		if (this->m_iter != this->m_clients.end())
		{
			rc = sendto(this->m_sock, ndm.asChar, ND_SIZE, 0, (SOCKADDR*)&this->m_iter, remoteAddrLen);
			LeaveCriticalSection(&this->m_critSec); //OUT

			if (rc == SOCKET_ERROR)
			{
				LOG->logMsg(LT_ERROR, "Couldn't send NDM to client! Reason: %d", WSAGetLastError());
				ret = false;
			}
		}
		else
		{
			LeaveCriticalSection(&this->m_critSec);  //OUT
			LOG->logMsg(LT_WARNING, "Suspecting hack attempt.");
			LOG->logMsg(LT_STATUS, "Unknown client!");
			ret = false;
		}

		return ret;
		*/
		return true;
	}

	bool ClientSocket::stopQueuing()
	{
		//this->m_stop = true;
		return true;
	}

	/*
	*	Server Socket
	************************/

	ServerSocket::ServerSocket(const NetworkManager& netMan, 
		const unsigned short port, const unsigned long addr)
		:Socket(netMan),m_stop(false),m_idCounter(1)
	{
		InitializeCriticalSection(&this->m_critSec);

		if (Socket::isInitialized())
		{
			SOCKADDR_IN address;
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = addr;
			address.sin_port = port;
			long rc;
			rc = bind(this->m_sock, (SOCKADDR*)&address, sizeof(SOCKADDR_IN));

			if (rc == SOCKET_ERROR)
			{
				this->m_isListening = false;
				LOG->logMsg(LT_ERROR, "Failed to bind UDP Sock: %d", WSAGetLastError()); 
				return;
			}
			else
			{
				this->m_isListening = true;
			}

		}
		else
		{
			this->m_isListening = false;
		}
	}

	ServerSocket::~ServerSocket()
	{
		DeleteCriticalSection(&this->m_critSec);
	}

	void ServerSocket::fillQueue(NetworkQueueTS& nq)
	{
		SOCKADDR_IN remote_address;
		CLIENT_UID uid;
		int remoteAddrLen = sizeof(SOCKADDR_IN);
		unsigned long rc;
		NetworkDataMessage ndm;

		LOG->logMsg(LT_STATUS, "Yippie, Thread lives!");

		while (!this->m_stop)
		{
			rc = recvfrom(this->m_sock, ndm.asChar, ND_SIZE, MSG_PEEK, (SOCKADDR*)&remote_address, &remoteAddrLen);

			if (rc == SOCKET_ERROR)
			{
				LOG->logMsg(LT_ERROR, "recvfrom failed: %d", rc);
				continue;
			}
			else if(rc < ND_SIZE)
			{
				continue;
			}
			// Should be standard:
			else
			{
				LOG->logMsg(LT_STATUS, "Receiving packet!");
				rc = recvfrom(this->m_sock, ndm.asChar, ND_SIZE, 0, (SOCKADDR*)&remote_address, &remoteAddrLen);
				// If this is the first packet from the client:
				if (ndm.NetData.UID == 0)
				{
					ndm.NetData.UID = this->m_idCounter;
					this->m_idCounter++;

					EnterCriticalSection(&this->m_critSec);
					this->m_clients.insert(std::make_pair(ndm.NetData.UID, remote_address));
					LeaveCriticalSection(&this->m_critSec);

					if (!this->sendNDM(ndm))
					{
						LOG->logMsg(LT_CRITICAL, "UID assignment failed!");
					}
				}
				else
				{
					nq.pushBack(ndm);
				}
			}
		}
		LOG->logMsg(LT_STATUS, "Server queue stops filling!");
	}

	const bool ServerSocket::sendNDM(const NetworkDataMessage& ndm)
	{
		bool ret = true;
		unsigned long rc;
		int remoteAddrLen = sizeof(SOCKADDR_IN);

		EnterCriticalSection(&this->m_critSec); //IN

		this->m_iter = this->m_clients.find(ndm.NetData.UID);
		// Should pracitically always be true:
		if (this->m_iter != this->m_clients.end())
		{
			rc = sendto(this->m_sock, ndm.asChar, ND_SIZE, 0, (SOCKADDR*)&this->m_iter, remoteAddrLen);
			LeaveCriticalSection(&this->m_critSec); //OUT

			if (rc == SOCKET_ERROR)
			{
				LOG->logMsg(LT_ERROR, "Couldn't send NDM to client! Reason: %d", WSAGetLastError());
				ret = false;
			}
		}
		else
		{
			LeaveCriticalSection(&this->m_critSec);  //OUT
			LOG->logMsg(LT_WARNING, "Suspecting hack attempt.");
			LOG->logMsg(LT_STATUS, "Unknown client!");
			ret = false;
		}

		return ret;
	}

	bool ServerSocket::stopQueuing()
	{
		this->m_stop = true;
		return true;
	}

	const bool ServerSocket::isListening() const
	{
		return this->m_isListening;
	}


}