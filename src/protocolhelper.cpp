#include "protocolhelper.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include "../protos/packet.pb.h"

void WriteProtocolMessage(google::protobuf::io::CodedOutputStream * out, matchmaking::Packet * packet) {
	out->WriteVarint32(packet->ByteSize());

	packet->SerializeToCodedStream(out);
}

std::vector<uint8_t> GetProtocolMessage(matchmaking::Packet * packet) {
	int32_t packet_length = packet->ByteSize();

	int32_t length_length = google::protobuf::io::CodedOutputStream::VarintSize32(packet_length);

	int32_t total_packet_length = length_length + packet_length;

	std::vector<uint8_t> buffer(total_packet_length);

	google::protobuf::io::ZeroCopyOutputStream* raw_output = new google::protobuf::io::ArrayOutputStream(buffer.data(), total_packet_length);
	google::protobuf::io::CodedOutputStream* coded_output = new google::protobuf::io::CodedOutputStream(raw_output);

	WriteProtocolMessage(coded_output, packet);

	delete coded_output;
	delete raw_output;

	return buffer;
}

matchmaking::Packet * ParseProtocolMessage(google::protobuf::io::CodedInputStream * in) {
	uint32_t packet_length;

	in->ReadVarint32(&packet_length);

	// protect against a lot of data sent
	// this will not be very effective, because the packet_length can be modified as well, but it will do for now
	google::protobuf::io::CodedInputStream::Limit limit = in->PushLimit(packet_length);

	matchmaking::Packet * packet = new matchmaking::Packet;

	if (!packet->ParseFromCodedStream(in)) {
		return nullptr; // we will return null if the packet cannot be parsed
	}

	in->PopLimit(limit);

	return packet;
}

matchmaking::Packet * ParseProtocolMessage(std::vector<uint8_t> * buffer) {
	google::protobuf::io::ZeroCopyInputStream * raw_input = new google::protobuf::io::ArrayInputStream(buffer->data(), buffer->size());

	google::protobuf::io::CodedInputStream * coded_input = new google::protobuf::io::CodedInputStream(raw_input);

	matchmaking::Packet * packet = ParseProtocolMessage(coded_input);

	delete coded_input;
	delete raw_input;

	return packet;
}