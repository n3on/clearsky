

#include "network/ping.h"

namespace clearsky
{
	ping_thread::ping_thread(const char* host)
	{
		this->io_serv = new boost::asio::io_service();
		this->ping = new pinger(*this->io_serv, host);
	}

	ping_thread::~ping_thread()
	{
		SAFE_DELETE(this->io_serv);
		SAFE_DELETE(this->ping);
	}

	void ping_thread::run()
	{
		try 
		{
			this->io_serv->run();
		}
		catch (std::exception& e)
		{
			LOG->logMsg(LT_ERROR, e.what());
		}

	}

	void ping_thread::start()
	{
		m_Thread = boost::thread(&ping_thread::run, this);
	}

	void ping_thread::join()
	{
		m_Thread.join();
	}

}