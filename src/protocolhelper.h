#pragma once

#include <google/protobuf/io/coded_stream.h>
#include "../protos/packet.pb.h"

void WriteProtocolMessage(google::protobuf::io::CodedOutputStream * out, matchmaking::Packet * packet);

std::vector<uint8_t> GetProtocolMessage(matchmaking::Packet * packet);

matchmaking::Packet * ParseProtocolMessage(google::protobuf::io::CodedInputStream * in);

matchmaking::Packet * ParseProtocolMessage(std::vector<uint8_t> * in);