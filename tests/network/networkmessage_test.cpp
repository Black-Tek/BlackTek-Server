#include "otpch.h"
#include "networkmessage.h"

#include <gtest/gtest.h>

TEST(NetworkMessage, IncomingReadsStopAtReceivedEnd)
{
	NetworkMessage message;
	message.getBodyBuffer()[0] = 0x42;
	message.getBuffer()[3] = 0x7F; // Stale pooled data must not be readable.
	message.setLength(3);
	EXPECT_EQ(message.getByte(), 0x42);
	EXPECT_EQ(message.getByte(), 0);
	EXPECT_TRUE(message.isOverrun());
	EXPECT_EQ(message.getBufferPosition(), 3);
}

TEST(NetworkMessage, TruncatedIntegerDoesNotReadPastReceivedEnd)
{
	NetworkMessage message;
	std::memset(message.getBuffer(), 0x7F, NETWORKMESSAGE_MAXSIZE);
	message.getBodyBuffer();
	message.setLength(3);
	EXPECT_EQ(message.get<uint32_t>(), 0U);
	EXPECT_TRUE(message.isOverrun());
	EXPECT_EQ(message.getBufferPosition(), 2);
}

TEST(NetworkMessage, TruncatedStringDoesNotExposePooledBytes)
{
	NetworkMessage message;
	auto body = message.getBodyBuffer();
	body[0] = 4;
	body[1] = 0;
	body[2] = 'a';
	message.setLength(5);
	EXPECT_TRUE(message.getString().empty());
	EXPECT_TRUE(message.isOverrun());
}

TEST(NetworkMessage, OutgoingPayloadCanBeReadBack)
{
	NetworkMessage message;
	message.add<uint32_t>(0x12345678);
	ASSERT_TRUE(message.setBufferPosition(0));
	EXPECT_EQ(message.get<uint32_t>(), 0x12345678U);
	EXPECT_FALSE(message.isOverrun());
	EXPECT_EQ(message.getByte(), 0);
	EXPECT_TRUE(message.isOverrun());
}

TEST(NetworkMessage, BackwardSkipCannotWrapCursor)
{
	NetworkMessage message;
	message.skipBytes(-9);
	EXPECT_TRUE(message.isOverrun());
	EXPECT_EQ(message.getBufferPosition(), NetworkMessage::INITIAL_BUFFER_POSITION);
}

TEST(NetworkMessage, PreviousByteCannotUnderflow)
{
	NetworkMessage message;
	message.skipBytes(-NetworkMessage::INITIAL_BUFFER_POSITION);
	EXPECT_EQ(message.getPreviousByte(), 0);
	EXPECT_TRUE(message.isOverrun());
	EXPECT_EQ(message.getBufferPosition(), 0);
}
