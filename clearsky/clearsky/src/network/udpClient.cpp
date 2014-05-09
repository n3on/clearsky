#include "core/precompiled.h"
#include "network/udpClient.h"

namespace clearsky
{
	UdpClientThread::UdpClientThread()
	{
		this->m_io_serv = new boost::asio::io_service();
		this->m_serv = new UdpClient(*this->m_io_serv);
	}

	UdpClientThread::~UdpClientThread()
	{
		SAFE_DELETE(this->m_io_serv);
		SAFE_DELETE(this->m_serv);
	}

	void UdpClientThread::run()
	{
		this->m_io_serv->run();
	}

	void UdpClientThread::start()
	{
		m_Thread = boost::thread(&UdpClientThread::run, this);
	}

	void UdpClientThread::join()
	{
		m_Thread.join();
	}

}