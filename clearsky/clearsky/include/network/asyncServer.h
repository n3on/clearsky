#pragma once
#ifndef _CLEARSKY_ASYNC_SERVER_H_
#define _CLEARSKY_ASYNC_SERVER_H


#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "network/networkIncludes.h"

using boost::asio::ip::tcp;

namespace clearsky
{

class CLEARSKY_API tcp_connection
	: public boost::enable_shared_from_this<tcp_connection>
{
public:
	typedef boost::shared_ptr<tcp_connection> pointer;

	static pointer create(boost::asio::io_service& io_service)
	{
		return pointer(new tcp_connection(io_service));
	}

	tcp::socket& socket()
	{
		return socket_;
	}

	void start()
	{
		message_ = "Yippie ya yay motherfucker!";

		boost::asio::async_write(socket_, boost::asio::buffer(message_),
			boost::bind(&tcp_connection::handle_write, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

private:
	tcp_connection(boost::asio::io_service& io_service)
		: socket_(io_service)
	{
	}

	void handle_write(const boost::system::error_code& /*error*/,
		size_t /*bytes_transferred*/)
	{
	}

	tcp::socket socket_;
	std::string message_;
};



class CLEARSKY_API tcp_server
{
public:
	tcp_server(boost::asio::io_service& io_service)
		: acceptor_(io_service, tcp::endpoint(tcp::v4(), 13))
	{
		start_accept();
	}

private:
	void start_accept()
	{
		tcp_connection::pointer new_connection =
			tcp_connection::create(acceptor_.io_service());

		acceptor_.async_accept(new_connection->socket(),
			boost::bind(&tcp_server::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
	}

	void handle_accept(tcp_connection::pointer new_connection,
		const boost::system::error_code& error)
	{
		if (!error)
		{
			new_connection->start();
			start_accept();
		}
	}

	tcp::acceptor acceptor_;
};

class CLEARSKY_API tcpserver_thread
{
private:
	boost::thread    m_Thread;
	boost::asio::io_service* io_serv;
	tcp_server* serv;

	void run();

public:
	tcpserver_thread();
	virtual ~tcpserver_thread();

	void start();
	void join();

};

}

#endif