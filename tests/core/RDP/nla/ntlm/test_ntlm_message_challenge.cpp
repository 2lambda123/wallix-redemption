/*
    This program is free software; you can redistribute it and/or modify it
     under the terms of the GNU General Public License as published by the
     Free Software Foundation; either version 2 of the License, or (at your
     option) any later version.

    This program is distributed in the hope that it will be useful, but
     WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
     Public License for more details.

    You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     675 Mass Ave, Cambridge, MA 02139, USA.

    Product name: redemption, a FLOSS RDP proxy
    Copyright (C) Wallix 2013
    Author(s): Christophe Grosjean, Raphael Zhou, Meng Tan
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestNtlmMessageChallenge
#include <boost/test/auto_unit_test.hpp>

#define LOGNULL

#include "RDP/nla/credssp.hpp"
#include "RDP/nla/ntlm/ntlm_message_challenge.hpp"

#include "check_sig.hpp"

BOOST_AUTO_TEST_CASE(TestChallenge)
{
    StaticOutStream<65536> s;
    // ===== NTLMSSP_CHALLENGE =====
    uint8_t packet2[] = {
        0x30, 0x81, 0x94, 0xa0, 0x03, 0x02, 0x01, 0x02,
        0xa1, 0x81, 0x8c, 0x30, 0x81, 0x89, 0x30, 0x81,
        0x86, 0xa0, 0x81, 0x83, 0x04, 0x81, 0x80, 0x4e,
        0x54, 0x4c, 0x4d, 0x53, 0x53, 0x50, 0x00, 0x02,
        0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x38,
        0x00, 0x00, 0x00, 0x35, 0x82, 0x8a, 0xe2, 0x26,
        0x6e, 0xcd, 0x75, 0xaa, 0x41, 0xe7, 0x6f, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
        0x00, 0x40, 0x00, 0x40, 0x00, 0x00, 0x00, 0x06,
        0x01, 0xb0, 0x1d, 0x00, 0x00, 0x00, 0x0f, 0x57,
        0x00, 0x49, 0x00, 0x4e, 0x00, 0x37, 0x00, 0x02,
        0x00, 0x08, 0x00, 0x57, 0x00, 0x49, 0x00, 0x4e,
        0x00, 0x37, 0x00, 0x01, 0x00, 0x08, 0x00, 0x57,
        0x00, 0x49, 0x00, 0x4e, 0x00, 0x37, 0x00, 0x04,
        0x00, 0x08, 0x00, 0x77, 0x00, 0x69, 0x00, 0x6e,
        0x00, 0x37, 0x00, 0x03, 0x00, 0x08, 0x00, 0x77,
        0x00, 0x69, 0x00, 0x6e, 0x00, 0x37, 0x00, 0x07,
        0x00, 0x08, 0x00, 0xa9, 0x8d, 0x9b, 0x1a, 0x6c,
        0xb0, 0xcb, 0x01, 0x00, 0x00, 0x00, 0x00
    };


    LOG(LOG_INFO, "=================================\n");
    s.out_copy_bytes(packet2, sizeof(packet2));

    uint8_t sig[20];
    get_sig(s, sig, sizeof(sig));

    InStream in_s(s.get_data(), s.get_offset());
    TSRequest ts_req2(in_s);

    BOOST_CHECK_EQUAL(ts_req2.version, 2);

    BOOST_CHECK_EQUAL(ts_req2.negoTokens.size(), 0x80);
    BOOST_CHECK_EQUAL(ts_req2.authInfo.size(), 0);
    BOOST_CHECK_EQUAL(ts_req2.pubKeyAuth.size(), 0);

    StaticOutStream<65536> to_send2;

    BOOST_CHECK_EQUAL(to_send2.get_offset(), 0);
    ts_req2.emit(to_send2);

    BOOST_CHECK_EQUAL(to_send2.get_offset(), 0x94 + 3);

    char message[1024];
    if (!check_sig(to_send2, message, (const char *)sig)){
        BOOST_CHECK_MESSAGE(false, message);
    }

    NTLMChallengeMessage ChallengeMsg;

    hexdump_c(ts_req2.negoTokens.get_data(), ts_req2.negoTokens.size());
    // ChallengeMsg.recv(ts_req2.negoTokens);

    InStream token(ts_req2.negoTokens.get_data(), ts_req2.negoTokens.size());
    ChallengeMsg.recv(token);

    BOOST_CHECK_EQUAL(ChallengeMsg.negoFlags.flags, 0xe28a8235);
    ChallengeMsg.negoFlags.print();

    BOOST_CHECK_EQUAL(ChallengeMsg.TargetName.len, 8);
    BOOST_CHECK_EQUAL(ChallengeMsg.TargetName.bufferOffset, 56);
    uint8_t targetname_match[] =
        "\x57\x00\x49\x00\x4e\x00\x37\x00";
    BOOST_CHECK_EQUAL(memcmp(targetname_match,
                             ChallengeMsg.TargetName.buffer.ostream.get_data(),
                             ChallengeMsg.TargetName.len),
                      0);
    // hexdump_c(ChallengeMsg.TargetName.buffer.ostream.get_data(),
    //           ChallengeMsg.TargetName.buffer.ostream.size());
    BOOST_CHECK_EQUAL(ChallengeMsg.TargetInfo.len, 64);
    BOOST_CHECK_EQUAL(ChallengeMsg.TargetInfo.bufferOffset, 64);
    uint8_t targetinfo_match[] =
        "\x02\x00\x08\x00\x57\x00\x49\x00\x4e\x00\x37\x00\x01\x00\x08\x00"
        "\x57\x00\x49\x00\x4e\x00\x37\x00\x04\x00\x08\x00\x77\x00\x69\x00"
        "\x6e\x00\x37\x00\x03\x00\x08\x00\x77\x00\x69\x00\x6e\x00\x37\x00"
        "\x07\x00\x08\x00\xa9\x8d\x9b\x1a\x6c\xb0\xcb\x01\x00\x00\x00\x00";
    BOOST_CHECK_EQUAL(memcmp(targetinfo_match,
                             ChallengeMsg.TargetInfo.buffer.ostream.get_data(),
                             ChallengeMsg.TargetInfo.len),
                      0);
    // hexdump_c(ChallengeMsg.TargetInfo.buffer.ostream.get_data(),
    //           ChallengeMsg.TargetInfo.buffer.ostream.size());
    InStream servChall(ChallengeMsg.serverChallenge, 8);
    uint64_t servchallengeinteger = servChall.in_uint64_le();
    BOOST_CHECK_EQUAL(servchallengeinteger, 8063485858206805542LL);

    // ChallengeMsg.version.print();

    ChallengeMsg.AvPairList.print();

    // // hexdump_c(to_send2.get_data(), to_send2.size());

    StaticOutStream<65535> tosend;
    ChallengeMsg.emit(tosend);

    NTLMChallengeMessage ChallengeMsgDuplicate;

    InStream in_tosend(tosend.get_data(), tosend.get_offset());
    ChallengeMsgDuplicate.recv(in_tosend);

    BOOST_CHECK_EQUAL(ChallengeMsgDuplicate.negoFlags.flags, 0xE28A8235);
    // ChallengeMsgDuplicate.negoFlags.print();

    BOOST_CHECK_EQUAL(ChallengeMsgDuplicate.TargetName.len, 8);
    BOOST_CHECK_EQUAL(ChallengeMsgDuplicate.TargetName.bufferOffset, 56);

    BOOST_CHECK_EQUAL(ChallengeMsgDuplicate.TargetInfo.len, 64);
    BOOST_CHECK_EQUAL(ChallengeMsgDuplicate.TargetInfo.bufferOffset, 64);
    InStream servChall2(ChallengeMsgDuplicate.serverChallenge, 8);
    uint64_t servchallengeinteger2 = servChall2.in_uint64_le();
    BOOST_CHECK_EQUAL(servchallengeinteger2, 8063485858206805542LL);

    // ChallengeMsgDuplicate.AvPairList.print();
}
