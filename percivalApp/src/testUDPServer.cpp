//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>
#include <iostream>
#include <string>
#include <iomanip>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>

using boost::asio::ip::udp;

typedef struct bufferInfo_t
{
	uint8_t*     addr;
	unsigned int length;
} BufferInfo;

typedef struct packetHeader_t
{
	uint32_t frameNumber;
	uint32_t packetNumberFlags;
} PacketHeader;

const uint32_t kStartOfFrameMarker = 1 << 31;
const uint32_t kEndOfFrameMarker   = 1 << 30;
const uint32_t kPacketNumberMask   = 0x3FFFFFFF;

int main()
{
  try
  {

    PacketHeader mPacketHeader;
    mPacketHeader.packetNumberFlags = 0;
    mPacketHeader.frameNumber = 0;
    uint32_t mCurrentFrameNumber;
    uint8_t payload[40000];

    boost::asio::io_service io_service;

    udp::socket socket(io_service, udp::endpoint(udp::v4(), 9876));

    for (;;)
    {
      boost::array<boost::asio::mutable_buffer, 3> recv_buf;
      recv_buf[0] = boost::asio::buffer((void*)&mPacketHeader, sizeof(mPacketHeader));
      recv_buf[1] = boost::asio::buffer(payload, 40000);
      recv_buf[2] = boost::asio::buffer((void*)&mCurrentFrameNumber, sizeof(mCurrentFrameNumber));
//      boost::array<char, 1> recv_buf;
      udp::endpoint remote_endpoint;
      boost::system::error_code error;
      socket.receive_from(recv_buf,
          remote_endpoint, 0, error);

      if (error && error != boost::asio::error::message_size)
        throw boost::system::system_error(error);
//      std::string text = boost::asio::buffer_cast<const char*>(recv_buf.data());
//      unsigned char* p1 = boost::asio::buffer_cast<unsigned char*>(recv_buf);


      //std::string text;
      //std::copy(recv_buf.begin(), recv_buf.begin()+1, std::back_inserter(text));
//      std::cout << "Packet Header: " << recv_buf[0] << std::endl;

      std::cout << "====================================" << std::endl;
      std::cout << "Packet Number: " << (mPacketHeader.packetNumberFlags & kPacketNumberMask) << std::endl;
      std::cout << "Packet SOF: " << ((mPacketHeader.packetNumberFlags & kStartOfFrameMarker)>>31) << std::endl;
      std::cout << "Packet EOF: " << ((mPacketHeader.packetNumberFlags & kEndOfFrameMarker)>>30) << std::endl;
      std::cout << "Packet Frame : " << mPacketHeader.frameNumber << std::endl;

    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
