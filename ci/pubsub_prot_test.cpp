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
    : public testing::TestWithParam<std::tuple<int, PubSubIdSize, PubSubPayloadParametersSize, PubsubProtQoS,
                                               PubsubProtCrc, PubSubPid, std::vector<uint8_t>>>
{
   public:
    MockPubSubTransportLayer transportLayer{};
    MockPubSubSystemUtils systemUtils{};
    PubSubProtocol protocol{ &transportLayer, &systemUtils };
};

INSTANTIATE_TEST_SUITE_P(PubSubTestName, PubSubTest,
                         testing::Values(std::make_tuple(64, PubSubIdSize::ID_1BYTE,
                                                         PubSubPayloadParametersSize::PAYLOAD_1BYTE,
                                                         PubsubProtQoS::QoS0, PubsubProtCrc::Crc_None,
                                                         PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x1, 0x0 })));

TEST_P(PubSubTest, _1)
{
    auto params                              = GetParam();
    int buffer_size                          = std::get<0>(params);
    PubSubIdSize id_size                     = std::get<1>(params);
    PubSubPayloadParametersSize payload_size = std::get<2>(params);
    PubsubProtQoS qos                        = std::get<3>(params);
    PubsubProtCrc crc                        = std::get<4>(params);
    PubSubPid pid                            = std::get<5>(params);
    std::vector<uint8_t> expected_header     = std::get<6>(params);

    PubSubHeaderV1 header = {
        .version = 0, .id_size = id_size, .payload_length_size = payload_size, .qos = qos, .crc = crc, .pid = pid
    };

    std::vector<uint8_t> parsed_header{};
    parsed_header.reserve(buffer_size);
    ASSERT_EQ(packHeaderV1(header, parsed_header.data(), parsed_header.capacity()), 2);
}
