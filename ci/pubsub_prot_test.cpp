#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "protocols/publish_subscribe/pubsub_prot.hpp"
#include <vector>

using namespace ::testing;


struct MockPubSubTransportLayer : public IPubSubTransportLayer {
    MOCK_METHOD(int, write, (const uint8_t* data, int size), (override));
    MOCK_METHOD(int, read, (uint8_t* data, int size), (override));
};

struct MockPubSubSystemUtils : public IPubSubSystemUtils {
    MOCK_METHOD(void, sleep, (int timeout_ms), (override));
    MOCK_METHOD(int, time, (), (override));
};

class PubSubTest :
    public testing::TestWithParam<std::tuple<int, int, PubSubIdSize, PubSubPayloadParametersSize, PubsubProtQoS, PubsubProtCrc, std::vector<uint8_t>, std::vector<uint8_t>>> {
public:
    MockPubSubTransportLayer transportLayer{};
    MockPubSubSystemUtils systemUtils{};
    PubSubProtocol protocol{&transportLayer, &systemUtils};
};

INSTANTIATE_TEST_SUITE_P(PubSubTestName,
                         PubSubTest,
                         testing::Values(
                             std::make_tuple(1000, 5, PubSubIdSize::ID_1BYTE, PubSubPayloadParametersSize::PAYLOAD_1BYTE,
                                PubsubProtQoS::QoS0, PubsubProtCrc::Crc_None, 
                                std::vector<uint8_t>{0x1, 0x2, 0x3, 0x4},
                                std::vector<uint8_t>{0x0, 0x0, 0x0, 0x1, 0x2, 0x3, 0x4})
                         ));

TEST_P(PubSubTest, _1)
{
    auto params = GetParam();
    int timeout_ms = std::get<0>(params);
    int step_ms = std::get<1>(params);
    PubSubIdSize id_size = std::get<2>(params);
    PubSubPayloadParametersSize payload_size = std::get<3>(params);
    PubsubProtQoS qos = std::get<4>(params);
    PubsubProtCrc crc = std::get<5>(params);
    std::vector<uint8_t> to_send_data = std::get<6>(params);
    std::vector<uint8_t> output_data = std::get<7>(params);

    TransmissionParameters transmission_parameters = 
    {
        .timeout_ms = timeout_ms,
        .step_ms = step_ms,
        .id_size = id_size,
        .payload_length_size = payload_size,
        .qos = qos,
        .crc = crc,
    };
    EXPECT_CALL(transportLayer, write(_, _)).Times(1);
    protocol.publish(1, to_send_data.data(), to_send_data.size(), transmission_parameters);
}



