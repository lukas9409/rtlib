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

class HeaderTest
    : public testing::TestWithParam<std::tuple<PubSubIdSize, PubSubPayloadParametersSize, PubsubProtQoS, PubsubProtCrc,
                                               PubSubPid, std::vector<uint8_t>>>
{
   public:
    MockPubSubTransportLayer transportLayer{};
    MockPubSubSystemUtils systemUtils{};
    PubSubProtocol protocol{ &transportLayer, &systemUtils };
};

INSTANTIATE_TEST_SUITE_P(
    HeaderTest, HeaderTest,
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
                        PubsubProtCrc::Crc_32, PubSubPid::PUBLISH, std::vector<uint8_t>{ 0x0, 0x30 }),
        std::make_tuple(PubSubIdSize::ID_1BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBACK, std::vector<uint8_t>{ 0x0, 0x1 }),
        std::make_tuple(PubSubIdSize::ID_1BYTE, PubSubPayloadParametersSize::PAYLOAD_0BYTE, PubsubProtQoS::QoS0,
                        PubsubProtCrc::Crc_None, PubSubPid::PUBNACK, std::vector<uint8_t>{ 0x0, 0x2 })

            ));

TEST_P(HeaderTest, PackV1)
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

TEST_P(HeaderTest, UnpackV1)
{
    auto params                              = GetParam();
    PubSubIdSize id_size                     = std::get<0>(params);
    PubSubPayloadParametersSize payload_size = std::get<1>(params);
    PubsubProtQoS qos                        = std::get<2>(params);
    PubsubProtCrc crc                        = std::get<3>(params);
    PubSubPid pid                            = std::get<4>(params);
    std::vector<uint8_t> input_header        = std::get<5>(params);

    PubSubHeaderV1 header{};

    ASSERT_EQ(unpackHeaderV1(&header, input_header.data(), 2), 2);
    ASSERT_EQ(id_size, header.id_size);
    ASSERT_EQ(payload_size, header.payload_length_size);
    ASSERT_EQ(qos, header.qos);
    ASSERT_EQ(crc, header.crc);
    ASSERT_EQ(pid, header.pid);
}

class MessageIdTest : public testing::TestWithParam<std::tuple<PubSubIdSize, uint64_t, std::vector<uint8_t>>>
{
   public:
    MockPubSubTransportLayer transportLayer{};
    MockPubSubSystemUtils systemUtils{};
    PubSubProtocol protocol{ &transportLayer, &systemUtils };
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    MessageIdTest, MessageIdTest,
    testing::Values(
        std::make_tuple(PubSubIdSize::ID_1BYTE, 0, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_1BYTE, 0x1, std::vector<uint8_t>{ 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_1BYTE, 0xf, std::vector<uint8_t>{ 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_1BYTE, 0xff, std::vector<uint8_t>{ 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_2BYTE, 0x1, std::vector<uint8_t>{ 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_2BYTE, 0xff, std::vector<uint8_t>{ 0x0, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_2BYTE, 0x100, std::vector<uint8_t>{ 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_2BYTE, 0xfff, std::vector<uint8_t>{ 0xf, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_2BYTE, 0xffff, std::vector<uint8_t>{ 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_3BYTE, 0x1, std::vector<uint8_t>{ 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_3BYTE, 0x100, std::vector<uint8_t>{ 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_3BYTE, 0x10000, std::vector<uint8_t>{ 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_3BYTE, 0xffffff, std::vector<uint8_t>{ 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_4BYTE, 0x1, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_4BYTE, 0x100, std::vector<uint8_t>{ 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_4BYTE, 0x10000, std::vector<uint8_t>{ 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_4BYTE, 0x1000000, std::vector<uint8_t>{ 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_4BYTE, 0xffffffff, std::vector<uint8_t>{ 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_5BYTE, 0x1, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_5BYTE, 0x100, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_5BYTE, 0x10000, std::vector<uint8_t>{ 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_5BYTE, 0x1000000, std::vector<uint8_t>{ 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_5BYTE, 0x100000000, std::vector<uint8_t>{ 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_5BYTE, 0xffffffffff, std::vector<uint8_t>{ 0xff, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_6BYTE, 0x1, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_6BYTE, 0x100, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_6BYTE, 0x10000, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_6BYTE, 0x1000000, std::vector<uint8_t>{ 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_6BYTE, 0x100000000, std::vector<uint8_t>{ 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_6BYTE, 0x10000000000, std::vector<uint8_t>{ 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_6BYTE, 0xffffffffffff, std::vector<uint8_t>{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_7BYTE, 0x1, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_7BYTE, 0x100, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_7BYTE, 0x10000, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_7BYTE, 0x1000000, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_7BYTE, 0x100000000, std::vector<uint8_t>{ 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_7BYTE, 0x10000000000, std::vector<uint8_t>{ 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_7BYTE, 0x1000000000000, std::vector<uint8_t>{ 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_7BYTE, 0xffffffffffffff, std::vector<uint8_t>{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_8BYTE, 0x1, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1 }),
        std::make_tuple(PubSubIdSize::ID_8BYTE, 0x100, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_8BYTE, 0x10000, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_8BYTE, 0x1000000, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_8BYTE, 0x100000000, std::vector<uint8_t>{ 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_8BYTE, 0x10000000000, std::vector<uint8_t>{ 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_8BYTE, 0x1000000000000, std::vector<uint8_t>{ 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_8BYTE, 0x100000000000000, std::vector<uint8_t>{ 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubIdSize::ID_8BYTE, 0xffffffffffffffff, std::vector<uint8_t>{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff })

    ));
// clang-format on

TEST_P(MessageIdTest, PackV1)
{
    auto params                          = GetParam();
    PubSubIdSize id_size                 = std::get<0>(params);
    uint64_t message_id                  = std::get<1>(params);
    std::vector<uint8_t> expected_output = std::get<2>(params);

    PubSubHeaderV1 header = { .id_size = id_size };

    uint8_t parsed_header[8]{};
    ASSERT_EQ(packMessageIdV1(header, message_id, parsed_header, 8), id_size + 1);
    ASSERT_THAT(parsed_header, ElementsAreArray(expected_output));
}

TEST_P(MessageIdTest, UnpackV1)
{
    auto params                  = GetParam();
    PubSubIdSize id_size         = std::get<0>(params);
    uint64_t expected_message_id = std::get<1>(params);
    std::vector<uint8_t> intput  = std::get<2>(params);

    PubSubHeaderV1 header = { .id_size = id_size };

    uint64_t message_id{};
    ASSERT_EQ(unpackMessageIdV1(header, &message_id, intput.data(), intput.size()), id_size + 1);
    ASSERT_EQ(expected_message_id, message_id);
}

class PayloadSizeTest
    : public testing::TestWithParam<std::tuple<PubSubPayloadParametersSize, int, std::vector<uint8_t>>>
{
   public:
    MockPubSubTransportLayer transportLayer{};
    MockPubSubSystemUtils systemUtils{};
    PubSubProtocol protocol{ &transportLayer, &systemUtils };
};

INSTANTIATE_TEST_SUITE_P(
    PayloadSizeTest, PayloadSizeTest,
    testing::Values(
        std::make_tuple(PubSubPayloadParametersSize::PAYLOAD_0BYTE, 0, std::vector<uint8_t>{ 0x0, 0x0, 0x0 }),
        std::make_tuple(PubSubPayloadParametersSize::PAYLOAD_1BYTE, 0x1, std::vector<uint8_t>{ 0x1, 0x0, 0x0 }),
        std::make_tuple(PubSubPayloadParametersSize::PAYLOAD_1BYTE, 0xf, std::vector<uint8_t>{ 0xf, 0x0, 0x0 }),
        std::make_tuple(PubSubPayloadParametersSize::PAYLOAD_1BYTE, 0xff, std::vector<uint8_t>{ 0xff, 0x0, 0x0 }),
        std::make_tuple(PubSubPayloadParametersSize::PAYLOAD_2BYTE, 0x1, std::vector<uint8_t>{ 0x0, 0x1, 0x0 }),
        std::make_tuple(PubSubPayloadParametersSize::PAYLOAD_2BYTE, 0x100, std::vector<uint8_t>{ 0x1, 0x0, 0x0 }),
        std::make_tuple(PubSubPayloadParametersSize::PAYLOAD_2BYTE, 0xffff, std::vector<uint8_t>{ 0xff, 0xff, 0x0 }),
        std::make_tuple(PubSubPayloadParametersSize::PAYLOAD_3BYTE, 0x1, std::vector<uint8_t>{ 0x0, 0x0, 0x1 }),
        std::make_tuple(PubSubPayloadParametersSize::PAYLOAD_3BYTE, 0x100, std::vector<uint8_t>{ 0x0, 0x1, 0x0 }),
        std::make_tuple(PubSubPayloadParametersSize::PAYLOAD_3BYTE, 0x10000, std::vector<uint8_t>{ 0x1, 0x0, 0x0 }),
        std::make_tuple(PubSubPayloadParametersSize::PAYLOAD_3BYTE, 0xffffff, std::vector<uint8_t>{ 0xff, 0xff, 0xff })

            ));

TEST_P(PayloadSizeTest, PackV1)
{
    auto params                              = GetParam();
    PubSubPayloadParametersSize payload_size = std::get<0>(params);
    int value                                = std::get<1>(params);
    std::vector<uint8_t> expected_output     = std::get<2>(params);

    PubSubHeaderV1 header = { .payload_length_size = payload_size };

    uint8_t parsed_header[3]{};
    ASSERT_EQ(packPayloadSizeV1(header, value, parsed_header, 3), payload_size);
    ASSERT_THAT(parsed_header, ElementsAreArray(expected_output));
}

TEST_P(PayloadSizeTest, UnpackV1)
{
    auto params                              = GetParam();
    PubSubPayloadParametersSize payload_size = std::get<0>(params);
    int expected_value                       = std::get<1>(params);
    std::vector<uint8_t> input               = std::get<2>(params);

    PubSubHeaderV1 header = { .payload_length_size = payload_size };
    int value{};

    ASSERT_EQ(unpackPayloadSizeV1(header, &value, input.data(), input.size()), payload_size);
    ASSERT_EQ(expected_value, value);
}

class PayloadTest : public testing::TestWithParam<std::tuple<std::vector<uint8_t>>>
{
   public:
    MockPubSubTransportLayer transportLayer{};
    MockPubSubSystemUtils systemUtils{};
    PubSubProtocol protocol{ &transportLayer, &systemUtils };
};

INSTANTIATE_TEST_SUITE_P(
    PayloadTest, PayloadTest,
    testing::Values(std::make_tuple(std::vector<uint8_t>{}), std::make_tuple(std::vector<uint8_t>{ 0x1 }),
                    std::make_tuple(std::vector<uint8_t>{ 0xf }), std::make_tuple(std::vector<uint8_t>{ 0xff }),
                    std::make_tuple(std::vector<uint8_t>{ 0x0, 0x1, 0x0 }),
                    std::make_tuple(std::vector<uint8_t>{ 0xff, 0xff, 0xff }),
                    std::make_tuple(std::vector<uint8_t>(100, 0xff)), std::make_tuple(std::vector<uint8_t>(100, 0x0)),
                    std::make_tuple(std::vector<uint8_t>(10000, 0xff))

                        ));

TEST_P(PayloadTest, PackV1)
{
    auto params                  = GetParam();
    std::vector<uint8_t> payload = std::get<0>(params);

    uint8_t output[payload.size()]{};
    std::vector<uint8_t> output_vector{};

    ASSERT_EQ(packPayloadV1(payload.data(), payload.size(), output, payload.size()), payload.size());
    output_vector.assign(output, output + payload.size());
    ASSERT_EQ(payload, output_vector);
}

TEST_P(PayloadTest, UnpackV1)
{
    auto params                = GetParam();
    std::vector<uint8_t> input = std::get<0>(params);

    uint8_t payload[input.size()]{};
    std::vector<uint8_t> payload_vector{};

    ASSERT_EQ(unpackPayloadV1(payload, input.size(), input.data(), input.size()), input.size());
    payload_vector.assign(payload, payload + input.size());
    ASSERT_EQ(input, payload_vector);
}

class CrcTest : public testing::TestWithParam<std::tuple<std::vector<uint8_t>, PubsubProtCrc>>
{
   public:
    MockPubSubTransportLayer transportLayer{};
    MockPubSubSystemUtils systemUtils{};
    PubSubProtocol protocol{ &transportLayer, &systemUtils };
};

INSTANTIATE_TEST_SUITE_P(CrcTest, CrcTest,
                         testing::Values(std::make_tuple(std::vector<uint8_t>{ 0x1, 0x2, 0x3 },
                                                         PubsubProtCrc::Crc_None),
                                         std::make_tuple(std::vector<uint8_t>{ 0x1, 0x2, 0x3 }, PubsubProtCrc::Crc_8),
                                         std::make_tuple(std::vector<uint8_t>{ 0x1, 0x2, 0x3 }, PubsubProtCrc::Crc_16),
                                         std::make_tuple(std::vector<uint8_t>{ 0x1, 0x2, 0x3 }, PubsubProtCrc::Crc_32),
                                         std::make_tuple(std::vector<uint8_t>(1000, 0xab), PubsubProtCrc::Crc_None),
                                         std::make_tuple(std::vector<uint8_t>(1000, 0xab), PubsubProtCrc::Crc_8),
                                         std::make_tuple(std::vector<uint8_t>(1000, 0xab), PubsubProtCrc::Crc_16),
                                         std::make_tuple(std::vector<uint8_t>(1000, 0xab), PubsubProtCrc::Crc_32)

                                             ));

TEST_P(CrcTest, CalculateAndPackV1)
{
    auto params                  = GetParam();
    std::vector<uint8_t> payload = std::get<0>(params);
    PubsubProtCrc crc_type       = std::get<1>(params);

    PubSubHeaderV1 header = { .crc = crc_type };

    uint8_t crc_size = 0;
    if(crc_type == PubsubProtCrc::Crc_32)
    {
        crc_size = 4;
    }
    else
    {
        crc_size = crc_type;
    }

    uint8_t crc_output[4]{};
    ASSERT_EQ(calculateAndPackCrcV1(header, payload.data(), payload.size(), crc_output, 4), crc_size);

    for(int i = 0; i < crc_size; ++i)
    {
        payload.push_back(crc_output[i]);
    }

    ASSERT_EQ(true, verifyCrcV1(header, payload.data(), payload.size()));
}