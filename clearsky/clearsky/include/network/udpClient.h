#ifndef _UDP_CLIENT_H_
#define _UDP_CLIENT_H_

#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "network/networkIncludes.h"

using boost::asio::ip::udp;

namespace clearsky
{

using boost::asio::ip::udp;

class CLEARSKY_API UdpClient
{
public:
  UdpClient(boost::asio::io_service& io_service)
    : m_socket(io_service)
  {
	m_io_serv = &io_service;
    start_receive();
	send_something();
  }

  void send_something()
  {
	LOG->logMsg(LT_STATUS, "Sending something from client class...");
	udp::resolver resolver(*m_io_serv);
    udp::resolver::query query(udp::v4(), "localhost", "13",  boost::asio::ip::resolver_query_base::numeric_service);
    udp::resolver::iterator iterator = resolver.resolve(query);

	boost::shared_ptr<std::string> message(
          new std::string("Hello World!"));

      m_socket.async_send_to(boost::asio::buffer(*message), *iterator,
          boost::bind(&UdpClient::handle_send, this, message,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

  }

private:
  void start_receive()
  {
    m_socket.async_receive_from(
        boost::asio::buffer(m_recv_buffer), m_remote_endpoint,
        boost::bind(&UdpClient::handle_receive, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

  void handle_receive(const boost::system::error_code& error,
      std::size_t /*bytes_transferred*/)
  {
    if (!error || error == boost::asio::error::message_size)
    {
     /* boost::shared_ptr<std::string> message(
          new std::string("Hello World!"));

      m_socket.async_send_to(boost::asio::buffer(*message), m_remote_endpoint,
          boost::bind(&UdpClient::handle_send, this, message,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

		*/
      start_receive();
    }
  }

  void handle_send(boost::shared_ptr<std::string> /*message*/,
      const boost::system::error_code& /*error*/,
      std::size_t /*bytes_transferred*/)
  {
  }

  udp::socket m_socket;
  udp::endpoint m_remote_endpoint;
  boost::array<char, 1> m_recv_buffer;
  boost::asio::io_service* m_io_serv;
};


class CLEARSKY_API UdpClientThread
{
private:
	boost::thread    m_Thread;
	boost::asio::io_service* m_io_serv;
	UdpClient* m_serv;

	void run();

public:
	UdpClientThread();
	virtual ~UdpClientThread();

	void start();
	void join();

};

}

#endif