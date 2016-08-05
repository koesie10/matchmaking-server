#include <ctime>
#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/text_format.h>

#include "../../protos/announce.pb.h"
#include "../../protos/packet.pb.h"
#include "../../protos/time.pb.h"

#include "../bytehelper.h"
#include "../protocolhelper.h"

using boost::asio::ip::udp;
using namespace google::protobuf::io;
using namespace google::protobuf;

namespace po = boost::program_options;

std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

// Returns the packet that should be sent back, or nullptr if none needs to be send back
// TODO: Move to a class
matchmaking::Packet * ParsePacket(matchmaking::Packet * packet) {
	Message * message;

	switch (packet->type()) {
	case 0x01:
		message = new matchmaking::Announce;
		break;
	case 0x02:
		message = new matchmaking::TimeRequest;
		break;
	default:
		std::cerr << "Unknown packet type";
		return nullptr;
	}

	if (!message->ParseFromString(packet->message())) {
		std::cerr << "Failed to parse embedded packet";
		return nullptr;
	}

	std::cout << "\nMessage of type " << int_to_hex(packet->type()) << " (" << message->GetDescriptor()->full_name() << ")\n========\n";

	message->PrintDebugString();
	
	std::cout << "\n";

	matchmaking::Packet * returnPacket = nullptr;

	switch (packet->type()) {
	case 0x02:
		matchmaking::TimeResponse response;
		response.set_time(make_daytime_string());

		returnPacket = new matchmaking::Packet;
		returnPacket->set_type(0x03);
		returnPacket->set_length(response.ByteSize());
		returnPacket->set_message(response.SerializeAsString());
		break;
	}

	delete message;
	std::cout << "Return packet: " << (returnPacket != nullptr ? "Yes" : "No") << "\n";

	return returnPacket;
}

class UdpServer
{
public:
	UdpServer(boost::asio::io_service& io_service, uint16_t port)
		: socket_(io_service, udp::v6())
	{
		// this option needs to be set between opening and binding
		// we use this option to also listen op IPv4 without needing to open two sockets
		boost::asio::ip::v6_only ipv6_only_option(false);
		socket_.set_option(ipv6_only_option);

		socket_.bind(udp::endpoint(udp::v6(), port));

		StartReceive();
	}

private:
	void StartReceive()
	{
		socket_.async_receive_from(
			boost::asio::null_buffers(),
			remote_endpoint_,
			boost::bind(
				&UdpServer::HandleReceive,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);
	}

	void HandleReceive(const boost::system::error_code& error, std::size_t bytes)
	{
		std::cout << "Received " << bytes << " bytes\n";
		if (!error || error == boost::asio::error::message_size)
		{
			if (bytes == 0) {
				int available = socket_.available();

				recv_buffer_.clear();
				recv_buffer_.resize(available);

				socket_.async_receive_from(
					boost::asio::buffer(recv_buffer_),
					remote_endpoint_,
					boost::bind(
						&UdpServer::HandleReceive,
						this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred
					)
				);
				
				return;
			}
			
			print_vector_as_hex(&recv_buffer_);

			matchmaking::Packet * packet = ParseProtocolMessage(&recv_buffer_);

			if (packet != nullptr) {
				std::string packetString = "";
				google::protobuf::TextFormat::PrintToString(*packet, &packetString);

				std::cout << "\nPacket\n========\n" << packetString;

				matchmaking::Packet * returnPacket = ParsePacket(packet);

				if (returnPacket != nullptr) {
					std::vector<uint8_t> message = GetProtocolMessage(returnPacket);

					delete returnPacket;

					socket_.async_send_to(
						// TODO: Confirm this is save. We are not using the vector directly due to a "Vector iterator not dereferencable" error
						// When checking memory usage, it does seem we are not leaving behind any allocations, so it should be safe
						boost::asio::buffer(message.data(), message.size()),
						remote_endpoint_,
						boost::bind(
							&UdpServer::HandleSend,
							this,
							message,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred
							)
						);
				}
			}
			else {
				std::cerr << "Failed to parse packet\n";
			}

			delete packet;

			StartReceive();
		}
		else {
			std::cerr << "We got an error: " << error.message();
		}
	}

	void HandleSend(std::vector<uint8_t> /*message*/,
		const boost::system::error_code& /*error*/,
		std::size_t /*bytes_transferred*/)
	{
	}

	udp::socket socket_;
	udp::endpoint remote_endpoint_;

	std::vector<uint8_t> recv_buffer_;
};

int main(int argc, char* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("port", po::value<std::uint16_t>()->default_value(54001), "set port to bind to")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}

	try
	{
		boost::asio::io_service io_service;
		UdpServer server(io_service, vm["port"].as<uint16_t>());
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}