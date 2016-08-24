/* 
  PlannerServer.cpp : TCP Server that returns result from sbmpo.

  Heavily based on boost blocking TCP echo server example.
*/ 

#include "stdafx.h" // ths is related to precompiled headers
#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <boost/asio.hpp>

#define PORTN (1234)

using boost::asio::ip::tcp;

const int max_length = 1024; // Need better strategy for this.

void session(tcp::socket sock)
{
	try
	{
		for (;;)
		{
			char data[max_length]; //need to define this better

			boost::system::error_code error;
			size_t length = sock.read_some(boost::asio::buffer(data), error); //data = response, length = size of data.
			if (error == boost::asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw boost::system::system_error(error); // Some other error.

			std::string data_string = std::string(data);
			std::cout << "Length: " << length << std::endl;
			std::cout << "Data: " << data_string.substr(0,length) << std::endl;
			boost::asio::write(sock, boost::asio::buffer(data, length)); //write back (async)
			//sock.write_some(boost::asio::buffer(data,6)); //non async
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in thread: " << e.what() << "\n";
	}
}

/*
	Listen for connections and spawn sessions at connect.
*/
void server(boost::asio::io_service& io_service, unsigned short port)
{
	tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), port));
	std::cout << "Listening on Port" << PORTN << std::endl;
	for (;;)
	{
		tcp::socket sock(io_service);
		a.accept(sock);
		std::thread(session, std::move(sock)).detach();
	}
}

int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_service io_service;
		
		server(io_service, PORTN);
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	

	return 0;
}