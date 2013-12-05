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
#define BOOST_TEST_MODULE TestNtlmAvPair
#include <boost/test/auto_unit_test.hpp>

#define LOGPRINT
#include "log.hpp"

#include "RDP/nla/credssp.hpp"
#include "RDP/nla/ntlm/ntlm_avpair.hpp"

#include "check_sig.hpp"

BOOST_AUTO_TEST_CASE(TestAvPair)
{
    NtlmAvPairList listAvPair;

    BOOST_CHECK_EQUAL(listAvPair.length(), 1);
    BOOST_CHECK_EQUAL(listAvPair.packet_length(), 4);

    const uint8_t tartempion[] = "NomDeDomaine";

    listAvPair.add(MsvAvNbDomainName, tartempion, sizeof(tartempion));

    BOOST_CHECK_EQUAL(listAvPair.length(), 2);
    BOOST_CHECK_EQUAL(listAvPair.packet_length(), 21);

    BStream stream;

    listAvPair.emit(stream);
    BOOST_CHECK_EQUAL(listAvPair.packet_length(), stream.size());
    listAvPair.print();


}


BOOST_AUTO_TEST_CASE(TestAvPairRecv)
{
    const uint8_t TargetInfo[] = {
        0x02, 0x00, 0x08, 0x00, 0x57, 0x00, 0x49, 0x00,
        0x4e, 0x00, 0x37, 0x00, 0x01, 0x00, 0x08, 0x00,
        0x57, 0x00, 0x49, 0x00, 0x4e, 0x00, 0x37, 0x00,
        0x04, 0x00, 0x08, 0x00, 0x77, 0x00, 0x69, 0x00,
        0x6e, 0x00, 0x37, 0x00, 0x03, 0x00, 0x08, 0x00,
        0x77, 0x00, 0x69, 0x00, 0x6e, 0x00, 0x37, 0x00,
        0x07, 0x00, 0x08, 0x00, 0xa9, 0x8d, 0x9b, 0x1a,
        0x6c, 0xb0, 0xcb, 0x01, 0x00, 0x00, 0x00, 0x00
    };
    BStream stream;
    stream.out_copy_bytes(TargetInfo, sizeof(TargetInfo));
    stream.mark_end();
    stream.rewind();

    NtlmAvPairList avpairlist;

    avpairlist.recv(stream);
    avpairlist.print();


}
