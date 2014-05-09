#include "core/precompiled.h"
#include "network/udpServer.h"

namespace clearsky
{
	UdpServerThread::UdpServerThread()
	{
		this->io_serv = new boost::asio::io_service();
		this->serv = new UdpServer(*this->io_serv);
	}

	UdpServerThread::~UdpServerThread()
	{
		SAFE_DELETE(this->io_serv);
		SAFE_DELETE(this->serv);
	}

	void UdpServerThread::run()
	{
		this->io_serv->run();
	}

	void UdpServerThread::start()
	{
		m_Thread = boost::thread(&UdpServerThread::run, this);
	}

	void UdpServerThread::join()
	{
		m_Thread.join();
	}

}