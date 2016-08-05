#include <fstream>
#include <iostream>
#include <sstream> // for ostringstream
#include <string>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include "../../protos/packet.pb.h"
#include "../../protos/time.pb.h"

#include "../bytehelper.h"
#include "../protocolhelper.h"
#include "socket_options.h"

using boost::asio::ip::udp;
using namespace google::protobuf::io;

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("name", po::value<std::string>(), "set name")
		("uid", po::value<int>(), "set uid")
		("hostname", po::value<std::string>()->default_value("localhost"), "set hostname to send the packet to")
		("port", po::value<std::uint16_t>()->default_value(54001), "set port to send the packet to")
		("use-ipv6", "use ipv6 to send the packet")
		("save-to-file", "save the packet to a file")
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
		matchmaking::TimeRequest request;

		if (vm.count("name")) {
			request.set_name(vm["name"].as<std::string>());
		}
		else {
			std::cout << "Enter name: ";

			std::getline(std::cin, *request.mutable_name());
		}

		if (vm.count("uid")) {
			request.set_uid(vm["uid"].as<int>());
		}
		else {
			std::cout << "Enter uid: ";
			int uid;
			std::cin >> uid;
			request.set_uid(uid);
		}

		int32_t message_length = request.ByteSize();
		int32_t packet_type = 0x02;

		matchmaking::Packet packet;

		packet.set_type(packet_type);
		packet.set_length(message_length);
		packet.set_message(request.SerializeAsString());

		time_t now = time(0);

		if (vm.count("save-to-file")) {
			std::ostringstream out;
			out << "announce" << now << ".bin";

			std::ofstream fs;
			fs.open(out.str().c_str(), std::fstream::out | std::fstream::binary);

			ZeroCopyOutputStream* file_raw_output = new OstreamOutputStream(&fs);
			CodedOutputStream* file_coded_output = new CodedOutputStream(file_raw_output);

			WriteProtocolMessage(file_coded_output, &packet);

			delete file_coded_output;
			delete file_raw_output;
			fs.close();

			std::cout << "Packet has been saved to " << out.str() << "\n";
		}
		else {
			std::vector<uint8_t> buffer = GetProtocolMessage(&packet);

			print_vector_as_hex(&buffer);

			udp protocol = udp::v4();

			if (vm.count("use-ipv6")) {
				protocol = udp::v6();
			}

			boost::asio::io_service io_service;

			udp::resolver resolver(io_service);
			udp::resolver::query query(protocol, vm["hostname"].as<std::string>(), boost::lexical_cast<std::string>(vm["port"].as<std::uint16_t>()));
			udp::endpoint receiver_endpoint = *resolver.resolve(query);

			udp::socket socket(io_service);

			std::cout << "Connecting to " << receiver_endpoint.address().to_string() << ":" << receiver_endpoint.port() << "\n";

			socket.open(protocol);

			socket.set_option(send_timeout(5000));
			socket.set_option(recv_timeout(5000));

			while (true) {
				socket.send_to(boost::asio::buffer(buffer), receiver_endpoint);

				udp::endpoint sender_endpoint;
				socket.receive_from(boost::asio::null_buffers(), sender_endpoint);

				std::vector<uint8_t> recv_buf(socket.available());

				socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

				matchmaking::Packet * packet = ParseProtocolMessage(&recv_buf);

				if (packet == nullptr) {
					std::cerr << "Failed to parse packet";
					return -1;
				}

				if (packet->type() == 0x03) {
					matchmaking::TimeResponse response;

					if (!response.ParseFromString(packet->message())) {
						std::cerr << "Failed to parse message";
						return -2;
					}

					std::cout << response.time();
				}
				else {
					std::cerr << "Invalid packet";
					return -3;
				}
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}