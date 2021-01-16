#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "protocols/publish_subscribe/pubsub_prot.hpp"
#include "protocols/publish_subscribe/common.h"
#include <vector>

using namespace ::testing;

struct MockPubSubTransportLayer : public IPubSubTransportLayer
{
    MOCK_METHOD(int, write, (const uint8_t * data, int size), (override));
    MOCK_METHOD(int, read, (uint8_t * data, int size), (override));
};

struct MockPubSubSystemUtils : public IPubSubSystemUtils
{
    MOCK_METHOD(void, sleep, (int timeout_ms), (override));
    MOCK_METHOD(int, time, (), (override));
};

class PubSubTest
    : public testing::TestWithParam<std::tuple<PubSubIdSize, PubSubPayloadParametersSize, PubsubProtQoS, PubsubProtCrc,
                                               PubSubPid, std::vector<uint8_t>>>
{
   public:
    MockPubSubTransportLayer transportLayer{};
    MockPubSubSystemUtils systemUtils{};
    PubSubProtocol protocol{ &transportLayer, &systemUtils };
};

INSTANTIATE_TEST_SUITE_P(
    PubSubTestName, PubSubTest,
    testing::Values(
        std::make_tuple(PubSubIdSize::ID_1BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_2BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x4, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_3BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x8, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_4BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0xC, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_5BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x10, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_6BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x14, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_7BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x18, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_8BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x1C, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_1BYTE, PubSubPayloadParametersSize::PAYLOAD_1BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x1, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_1BYTE, PubSubPayloadParametersSize::PAYLOAD_2BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x2, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_1BYTE, PubSubPayloadParametersSize::PAYLOAD_3BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x3, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_1BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS1,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x0, 0x40 }),
        std::make_tuple(PubSubIdSize::ID_1BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS2,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x0, 0x80 }),
        std::make_tuple(PubSubIdSize::ID_1BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_8, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x0, 0x10 }),
        std::make_tuple(PubSubIdSize::ID_1BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_16, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x0, 0x20 }),
        std::make_tuple(PubSubIdSize::ID_1BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_32, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x0, 0x30 })

            ));

TEST_P(PubSubTest, _1)
{
    auto params                              = GetParam();
    PubSubIdSize id_size                     = std::get<0>(params);
    PubSubPayloadParametersSize payload_size = std::get<1>(params);
    PubsubProtQoS qos                        = std::get<2>(params);
    PubsubProtCrc crc                        = std::get<3>(params);
    PubSubPid pid                            = std::get<4>(params);
    std::vector<uint8_t> expected_header     = std::get<5>(params);

    PubSubHeaderV1 header = {
        .id_size = id_size, .payload_length_size = payload_size, .qos = qos, .crc = crc, .pid = pid
    };

    uint8_t parsed_header[2]{};
    ASSERT_EQ(packHeaderV1(header, parsed_header, 2), 2);
    ASSERT_THAT(parsed_header, ElementsAreArray(expected_header));
}
