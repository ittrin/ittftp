//
// Created by skosohov on 11.02.24.
//

#ifndef ITTFTPC_TFTP_PACKET_H
#define ITTFTPC_TFTP_PACKET_H

#include <array>
#include <cstdint>
#include <vector>

#define MAX_TFTP_DATA_LEN 512

namespace itt {
namespace tftp {

enum class Opcode {
    Unknown = 0,
    ReadRequest,
    WriteRequest,
    Data,
    Acknowledgment,
    Error
};

enum class Mode {
    NetASCII,
    Octet,
    Mail
};

enum class ErrorCode {
    NotDefined,
    FileNotFound,
    AccessViolation,
    DiskFull,
    IllegalOperation,
    UnknownTransferID,
    FileAlreadyExists,
    NoSuchUser
};

using BlockNumber = std::uint16_t;
using Data = std::array<uint8_t, MAX_TFTP_DATA_LEN>;
using Bytes = std::vector<uint8_t>;
using String = std::string;

class Packet {
public:
    Opcode opcode() const noexcept { return opcode_; }
    Bytes rawData() const noexcept;
    virtual bool fromBytes(const Bytes& raw_data) = 0;
    virtual Bytes serialize() const = 0;
protected:
    explicit Packet(Opcode opcode) : opcode_(opcode) {}
private:
    Opcode opcode_;
};

class Request : public Packet {
public:
    String filename() const noexcept { return filename_; }
    Mode mode() const noexcept { return mode_; }
protected:
    explicit Request(Opcode opcode) : Packet(opcode), filename_(), mode_(Mode::NetASCII) {}
    Request(Opcode opcode, const String& filename, Mode mode) : Packet(opcode), filename_{filename}, mode_{mode} {}
private:
    String filename_;
    Mode mode_;
};

class ReadRequest : public Request {
public:
    ReadRequest() : Request(Opcode::ReadRequest) {}
    ReadRequest(const String& filename, Mode mode) : Request(Opcode::ReadRequest, filename, mode) {}
    bool fromBytes(const Bytes& raw_data) override;
};

class WriteRequest : public Request {
    WriteRequest() : Request(Opcode::WriteRequest) {}
    WriteRequest(const String& filename, Mode mode) : Request(Opcode::WriteRequest, filename, mode) {}
    bool fromBytes(const Bytes& raw_data) override;
};

class DataPacket : public Packet {
public:
    DataPacket() : Packet(Opcode::Data), block_number_(0), data_() {}
    DataPacket(BlockNumber block_number, const Data& data) : Packet(Opcode::Data), block_number_(block_number),
                                                             data_(data) {}
    bool fromBytes(const Bytes& raw_data) override;
private:
    BlockNumber block_number_;
    Data data_;
};

class AcknowledgmentPacket : public Packet {
public:
    AcknowledgmentPacket() : Packet(Opcode::Acknowledgment), block_number_(0) {}
    explicit AcknowledgmentPacket(BlockNumber block_number) : Packet(Opcode::Acknowledgment),
                                                              block_number_(block_number) {}
private:
    BlockNumber block_number_;
};

class ErrorPacket : public Packet {
public:
    ErrorPacket() : Packet(Opcode::Error), error_code_(ErrorCode::NotDefined), error_msg_() {}
    ErrorPacket(ErrorCode err_code, const String& err_msg) : Packet(Opcode::Error), error_code_(err_code),
                                                            error_msg_(err_msg) {}
private:
    ErrorCode error_code_;
    String error_msg_;
};

}
}

#endif //ITTFTPC_TFTP_PACKET_H
