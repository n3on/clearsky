

#include "network/asyncServer.h"

namespace clearsky
{
	tcpserver_thread::tcpserver_thread()
	{
		this->io_serv = new boost::asio::io_service();
		this->serv = new tcp_server(*this->io_serv);
	}

	tcpserver_thread::~tcpserver_thread()
	{
		SAFE_DELETE(this->io_serv);
		SAFE_DELETE(this->serv);
	}

	void tcpserver_thread::run()
	{
		this->io_serv->run();
	}

	void tcpserver_thread::start()
	{
		m_Thread = boost::thread(&tcpserver_thread::run, this);
	}

	void tcpserver_thread::join()
	{
		m_Thread.join();
	}

}