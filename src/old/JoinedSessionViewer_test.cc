/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include "stx/exception.h"
#include "stx/test/unittest.h"
#include "JoinedSessionViewer.h"
#include "logjoin/JoinedSession.pb.h"

using namespace fnord;
using namespace cm;

unsigned char test_session_bin[1759] = {
  0x82, 0x01, 0x89, 0x0b, 0x08, 0x01, 0x10, 0x02, 0x1a, 0x0d, 0x65, 0x2d,
  0x62, 0x6f, 0x6f, 0x6b, 0x20, 0x70, 0x79, 0x6a, 0x61, 0x6d, 0x61, 0x22,
  0x0d, 0x62, 0x6f, 0x6f, 0x6b, 0x20, 0x65, 0x20, 0x70, 0x79, 0x6a, 0x61,
  0x6d, 0x61, 0x28, 0x25, 0x30, 0x00, 0x38, 0x01, 0x40, 0x00, 0x50, 0x00,
  0x58, 0x01, 0x8a, 0x01, 0x23, 0x78, 0x00, 0x98, 0x01, 0x01, 0xa2, 0x01,
  0x0a, 0x70, 0x7e, 0x37, 0x34, 0x36, 0x34, 0x38, 0x39, 0x32, 0x33, 0xa8,
  0x01, 0xe1, 0xcb, 0xa4, 0x02, 0xb0, 0x01, 0x0a, 0xb8, 0x01, 0xae, 0x06,
  0xc0, 0x01, 0x85, 0x2a, 0x8a, 0x01, 0x21, 0x78, 0x00, 0x98, 0x01, 0x05,
  0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x34, 0x36, 0x34, 0x38, 0x30, 0x38, 0x36,
  0x36, 0xa8, 0x01, 0xb4, 0x4e, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26,
  0xc0, 0x01, 0xc1, 0x3b, 0x8a, 0x01, 0x23, 0x78, 0x00, 0x98, 0x01, 0x06,
  0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x37, 0x36, 0x30, 0x36, 0x39, 0x37, 0x36,
  0x33, 0xa8, 0x01, 0x95, 0xf7, 0xbe, 0x01, 0xb0, 0x01, 0x08, 0xb8, 0x01,
  0xb1, 0x26, 0xc0, 0x01, 0xc5, 0x3b, 0x8a, 0x01, 0x23, 0x78, 0x00, 0x98,
  0x01, 0x07, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x33, 0x37, 0x35, 0x37, 0x39,
  0x31, 0x37, 0x37, 0xa8, 0x01, 0xd2, 0xc5, 0xac, 0x01, 0xb0, 0x01, 0x08,
  0xb8, 0x01, 0xd1, 0x26, 0xc0, 0x01, 0xed, 0x26, 0x8a, 0x01, 0x23, 0x78,
  0x00, 0x98, 0x01, 0x08, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x35, 0x33, 0x37,
  0x34, 0x34, 0x31, 0x33, 0x39, 0xa8, 0x01, 0xa2, 0xa8, 0x87, 0x01, 0xb0,
  0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01, 0xc1, 0x3b, 0x8a, 0x01,
  0x23, 0x78, 0x00, 0x98, 0x01, 0x09, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x37,
  0x35, 0x39, 0x34, 0x33, 0x33, 0x31, 0x35, 0xa8, 0x01, 0x8f, 0xca, 0xfd,
  0x05, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01, 0xc1, 0x3b,
  0x8a, 0x01, 0x22, 0x78, 0x00, 0x98, 0x01, 0x0a, 0xa2, 0x01, 0x0a, 0x70,
  0x7e, 0x37, 0x37, 0x39, 0x36, 0x35, 0x36, 0x31, 0x31, 0xa8, 0x01, 0xc2,
  0xec, 0x04, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01, 0xb9,
  0x26, 0x8a, 0x01, 0x22, 0x78, 0x00, 0x98, 0x01, 0x0b, 0xa2, 0x01, 0x0a,
  0x70, 0x7e, 0x32, 0x39, 0x37, 0x34, 0x30, 0x31, 0x38, 0x31, 0xa8, 0x01,
  0xd7, 0xbc, 0x04, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01,
  0xbd, 0x3b, 0x8a, 0x01, 0x23, 0x78, 0x00, 0x98, 0x01, 0x0c, 0xa2, 0x01,
  0x0a, 0x70, 0x7e, 0x36, 0x37, 0x37, 0x37, 0x38, 0x31, 0x32, 0x33, 0xa8,
  0x01, 0xce, 0xcf, 0xe9, 0x04, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26,
  0xc0, 0x01, 0xbd, 0x3b, 0x8a, 0x01, 0x22, 0x78, 0x00, 0x98, 0x01, 0x0d,
  0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x37, 0x37, 0x39, 0x36, 0x36, 0x32, 0x32,
  0x33, 0xa8, 0x01, 0xc2, 0xec, 0x04, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xd1,
  0x26, 0xc0, 0x01, 0xd9, 0x26, 0x8a, 0x01, 0x22, 0x78, 0x00, 0x98, 0x01,
  0x0e, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x31, 0x33, 0x38, 0x38, 0x32, 0x39,
  0x35, 0x30, 0xa8, 0x01, 0xaa, 0xc3, 0x2d, 0xb0, 0x01, 0x08, 0xb8, 0x01,
  0xd1, 0x26, 0xc0, 0x01, 0xd9, 0x26, 0x8a, 0x01, 0x23, 0x78, 0x00, 0x98,
  0x01, 0x0f, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x37, 0x33, 0x34, 0x30, 0x34,
  0x34, 0x32, 0x33, 0xa8, 0x01, 0xab, 0xd6, 0x89, 0x07, 0xb0, 0x01, 0x08,
  0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01, 0xbd, 0x3b, 0x8a, 0x01, 0x23, 0x78,
  0x00, 0x98, 0x01, 0x10, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x32, 0x36, 0x36,
  0x30, 0x32, 0x37, 0x30, 0x31, 0xa8, 0x01, 0xf5, 0xaf, 0x91, 0x02, 0xb0,
  0x01, 0x0b, 0xb8, 0x01, 0xe6, 0x16, 0xc0, 0x01, 0xea, 0x16, 0x8a, 0x01,
  0x23, 0x78, 0x00, 0x98, 0x01, 0x11, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x34,
  0x31, 0x30, 0x37, 0x34, 0x35, 0x34, 0x36, 0xa8, 0x01, 0xe1, 0xcb, 0xa4,
  0x02, 0xb0, 0x01, 0x0a, 0xb8, 0x01, 0xae, 0x06, 0xc0, 0x01, 0x85, 0x2a,
  0x8a, 0x01, 0x23, 0x78, 0x00, 0x98, 0x01, 0x12, 0xa2, 0x01, 0x0a, 0x70,
  0x7e, 0x36, 0x37, 0x36, 0x32, 0x35, 0x33, 0x38, 0x37, 0xa8, 0x01, 0xc7,
  0xfd, 0xa4, 0x07, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01,
  0xc1, 0x3b, 0x8a, 0x01, 0x23, 0x78, 0x00, 0x98, 0x01, 0x13, 0xa2, 0x01,
  0x0a, 0x70, 0x7e, 0x36, 0x34, 0x32, 0x31, 0x31, 0x38, 0x37, 0x31, 0xa8,
  0x01, 0x9a, 0xa1, 0xdf, 0x05, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26,
  0xc0, 0x01, 0xc1, 0x3b, 0x8a, 0x01, 0x23, 0x78, 0x00, 0x98, 0x01, 0x14,
  0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x36, 0x33, 0x34, 0x38, 0x36, 0x32, 0x34,
  0x33, 0xa8, 0x01, 0x9a, 0xa1, 0xdf, 0x05, 0xb0, 0x01, 0x08, 0xb8, 0x01,
  0xb1, 0x26, 0xc0, 0x01, 0xbd, 0x3b, 0x8a, 0x01, 0x23, 0x78, 0x00, 0x98,
  0x01, 0x15, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x37, 0x32, 0x33, 0x39, 0x34,
  0x30, 0x33, 0x39, 0xa8, 0x01, 0xce, 0xcf, 0xe9, 0x04, 0xb0, 0x01, 0x08,
  0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01, 0xbd, 0x3b, 0x8a, 0x01, 0x23, 0x78,
  0x00, 0x98, 0x01, 0x16, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x37, 0x31, 0x36,
  0x32, 0x39, 0x37, 0x33, 0x39, 0xa8, 0x01, 0xe1, 0xcb, 0xa4, 0x02, 0xb0,
  0x01, 0x0c, 0xb8, 0x01, 0xda, 0x1d, 0xc0, 0x01, 0xee, 0x1d, 0x8a, 0x01,
  0x23, 0x78, 0x00, 0x98, 0x01, 0x17, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x37,
  0x32, 0x39, 0x32, 0x36, 0x32, 0x39, 0x39, 0xa8, 0x01, 0xd1, 0xa0, 0xc2,
  0x01, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01, 0xc1, 0x3b,
  0x8a, 0x01, 0x22, 0x78, 0x00, 0x98, 0x01, 0x18, 0xa2, 0x01, 0x0a, 0x70,
  0x7e, 0x34, 0x35, 0x36, 0x33, 0x35, 0x31, 0x33, 0x34, 0xa8, 0x01, 0xc2,
  0xec, 0x04, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01, 0xc1,
  0x3b, 0x8a, 0x01, 0x21, 0x78, 0x00, 0x98, 0x01, 0x19, 0xa2, 0x01, 0x09,
  0x70, 0x7e, 0x38, 0x37, 0x34, 0x31, 0x34, 0x34, 0x32, 0xa8, 0x01, 0xba,
  0x9a, 0x72, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xd1, 0x26, 0xc0, 0x01, 0xdd,
  0x26, 0x8a, 0x01, 0x21, 0x78, 0x00, 0x98, 0x01, 0x1a, 0xa2, 0x01, 0x09,
  0x70, 0x7e, 0x38, 0x35, 0x34, 0x31, 0x37, 0x31, 0x30, 0xa8, 0x01, 0xba,
  0x9a, 0x72, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xd1, 0x26, 0xc0, 0x01, 0xdd,
  0x26, 0x8a, 0x01, 0x21, 0x78, 0x00, 0x98, 0x01, 0x1b, 0xa2, 0x01, 0x09,
  0x70, 0x7e, 0x38, 0x35, 0x35, 0x32, 0x31, 0x39, 0x34, 0xa8, 0x01, 0xba,
  0x9a, 0x72, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01, 0xc1,
  0x3b, 0x8a, 0x01, 0x22, 0x78, 0x00, 0x98, 0x01, 0x1c, 0xa2, 0x01, 0x0a,
  0x70, 0x7e, 0x34, 0x32, 0x34, 0x30, 0x35, 0x33, 0x35, 0x30, 0xa8, 0x01,
  0xba, 0x9a, 0x72, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xd1, 0x26, 0xc0, 0x01,
  0xa2, 0x0e, 0x8a, 0x01, 0x22, 0x78, 0x00, 0x98, 0x01, 0x1d, 0xa2, 0x01,
  0x0a, 0x70, 0x7e, 0x34, 0x31, 0x34, 0x31, 0x35, 0x38, 0x33, 0x38, 0xa8,
  0x01, 0xba, 0x9a, 0x72, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xd1, 0x26, 0xc0,
  0x01, 0xa2, 0x0e, 0x8a, 0x01, 0x23, 0x78, 0x00, 0x98, 0x01, 0x1e, 0xa2,
  0x01, 0x0a, 0x70, 0x7e, 0x36, 0x31, 0x39, 0x32, 0x33, 0x36, 0x39, 0x35,
  0xa8, 0x01, 0x81, 0xf1, 0xef, 0x03, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1,
  0x26, 0xc0, 0x01, 0xc5, 0x26, 0x8a, 0x01, 0x22, 0x78, 0x00, 0x98, 0x01,
  0x1f, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x37, 0x37, 0x37, 0x36, 0x31, 0x38,
  0x35, 0x31, 0xa8, 0x01, 0x9a, 0xd1, 0x3c, 0xb0, 0x01, 0x08, 0xb8, 0x01,
  0xb1, 0x26, 0xc0, 0x01, 0xb9, 0x26, 0x8a, 0x01, 0x22, 0x78, 0x00, 0x98,
  0x01, 0x20, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x31, 0x33, 0x31, 0x32, 0x38,
  0x37, 0x34, 0x32, 0xa8, 0x01, 0xba, 0x9a, 0x72, 0xb0, 0x01, 0x08, 0xb8,
  0x01, 0xd1, 0x26, 0xc0, 0x01, 0xdd, 0x26, 0x8a, 0x01, 0x23, 0x78, 0x00,
  0x98, 0x01, 0x21, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x33, 0x31, 0x36, 0x32,
  0x39, 0x37, 0x33, 0x38, 0xa8, 0x01, 0xb9, 0xf3, 0x8e, 0x03, 0xb0, 0x01,
  0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01, 0xc1, 0x3b, 0x8a, 0x01, 0x23,
  0x78, 0x00, 0x98, 0x01, 0x22, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x37, 0x34,
  0x36, 0x34, 0x38, 0x39, 0x32, 0x33, 0xa8, 0x01, 0xe1, 0xcb, 0xa4, 0x02,
  0xb0, 0x01, 0x0a, 0xb8, 0x01, 0xae, 0x06, 0xc0, 0x01, 0x85, 0x2a, 0x8a,
  0x01, 0x23, 0x78, 0x00, 0x98, 0x01, 0x23, 0xa2, 0x01, 0x0a, 0x70, 0x7e,
  0x36, 0x31, 0x34, 0x39, 0x38, 0x37, 0x32, 0x37, 0xa8, 0x01, 0xe1, 0xcb,
  0xa4, 0x02, 0xb0, 0x01, 0x0a, 0xb8, 0x01, 0xae, 0x06, 0xc0, 0x01, 0x85,
  0x2a, 0x8a, 0x01, 0x21, 0x78, 0x00, 0x98, 0x01, 0x24, 0xa2, 0x01, 0x0a,
  0x70, 0x7e, 0x33, 0x36, 0x35, 0x33, 0x34, 0x33, 0x34, 0x35, 0xa8, 0x01,
  0xb4, 0x4e, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01, 0xc1,
  0x3b, 0x8a, 0x01, 0x12, 0x78, 0x00, 0x98, 0x01, 0x25, 0xa2, 0x01, 0x0a,
  0x70, 0x7e, 0x37, 0x36, 0x36, 0x36, 0x38, 0x32, 0x32, 0x33, 0x8a, 0x01,
  0x22, 0x78, 0x00, 0x98, 0x01, 0x26, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x32,
  0x33, 0x32, 0x34, 0x31, 0x31, 0x34, 0x35, 0xa8, 0x01, 0xa0, 0xe4, 0x05,
  0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01, 0xb9, 0x26, 0x8a,
  0x01, 0x23, 0x78, 0x00, 0x98, 0x01, 0x27, 0xa2, 0x01, 0x0a, 0x70, 0x7e,
  0x33, 0x34, 0x31, 0x36, 0x30, 0x37, 0x34, 0x31, 0xa8, 0x01, 0xd2, 0xc5,
  0xac, 0x01, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01, 0xc1,
  0x3b, 0x8a, 0x01, 0x12, 0x78, 0x00, 0x98, 0x01, 0x28, 0xa2, 0x01, 0x0a,
  0x70, 0x7e, 0x37, 0x34, 0x36, 0x35, 0x36, 0x33, 0x33, 0x39, 0x90, 0x01,
  0x8c, 0xec, 0x91, 0xa8, 0x05, 0xd8, 0x02, 0x00, 0xe0, 0x02, 0x00, 0xe8,
  0x02, 0x00, 0xf0, 0x02, 0x00, 0x82, 0x01, 0xc7, 0x02, 0x08, 0x02, 0x10,
  0x02, 0x1a, 0x0d, 0x65, 0x2d, 0x62, 0x6f, 0x6f, 0x6b, 0x20, 0x70, 0x79,
  0x6a, 0x61, 0x6d, 0x61, 0x22, 0x0d, 0x62, 0x6f, 0x6f, 0x6b, 0x20, 0x65,
  0x20, 0x70, 0x79, 0x6a, 0x61, 0x6d, 0x61, 0x28, 0x07, 0x30, 0x00, 0x38,
  0x01, 0x40, 0x00, 0x50, 0x00, 0x58, 0x01, 0x8a, 0x01, 0x22, 0x78, 0x00,
  0x98, 0x01, 0x05, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x31, 0x38, 0x33, 0x38,
  0x35, 0x37, 0x38, 0x31, 0xa8, 0x01, 0xba, 0x9a, 0x72, 0xb0, 0x01, 0x05,
  0xb8, 0x01, 0x9d, 0x43, 0xc0, 0x01, 0x96, 0x08, 0x8a, 0x01, 0x22, 0x78,
  0x00, 0x98, 0x01, 0x06, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x33, 0x33, 0x33,
  0x32, 0x30, 0x36, 0x35, 0x37, 0xa8, 0x01, 0xa0, 0xe4, 0x05, 0xb0, 0x01,
  0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01, 0xc1, 0x3b, 0x8a, 0x01, 0x22,
  0x78, 0x00, 0x98, 0x01, 0x07, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x34, 0x32,
  0x37, 0x37, 0x32, 0x32, 0x30, 0x36, 0xa8, 0x01, 0xba, 0x9a, 0x72, 0xb0,
  0x01, 0x08, 0xb8, 0x01, 0xd1, 0x26, 0xc0, 0x01, 0xa2, 0x0e, 0x8a, 0x01,
  0x23, 0x78, 0x00, 0x98, 0x01, 0x08, 0xa2, 0x01, 0x0a, 0x70, 0x7e, 0x37,
  0x34, 0x37, 0x38, 0x30, 0x35, 0x31, 0x31, 0xa8, 0x01, 0x9f, 0xb2, 0x88,
  0x09, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01, 0xd9, 0x3f,
  0x8a, 0x01, 0x23, 0x78, 0x00, 0x98, 0x01, 0x09, 0xa2, 0x01, 0x0a, 0x70,
  0x7e, 0x37, 0x34, 0x37, 0x32, 0x36, 0x37, 0x39, 0x31, 0xa8, 0x01, 0x9f,
  0xb2, 0x88, 0x09, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xb1, 0x26, 0xc0, 0x01,
  0xd9, 0x3f, 0x8a, 0x01, 0x22, 0x78, 0x00, 0x98, 0x01, 0x0a, 0xa2, 0x01,
  0x0a, 0x70, 0x7e, 0x34, 0x31, 0x35, 0x32, 0x37, 0x31, 0x30, 0x32, 0xa8,
  0x01, 0xba, 0x9a, 0x72, 0xb0, 0x01, 0x08, 0xb8, 0x01, 0xd1, 0x26, 0xc0,
  0x01, 0xa2, 0x0e, 0x8a, 0x01, 0x23, 0x78, 0x00, 0x98, 0x01, 0x01, 0xa2,
  0x01, 0x0a, 0x70, 0x7e, 0x37, 0x34, 0x36, 0x34, 0x38, 0x39, 0x32, 0x33,
  0xa8, 0x01, 0xe1, 0xcb, 0xa4, 0x02, 0xb0, 0x01, 0x0a, 0xb8, 0x01, 0xae,
  0x06, 0xc0, 0x01, 0x85, 0x2a, 0x90, 0x01, 0x93, 0xec, 0x91, 0xa8, 0x05,
  0xd8, 0x02, 0x00, 0xe0, 0x02, 0x00, 0xe8, 0x02, 0x00, 0xf0, 0x02, 0x00,
  0xda, 0x03, 0x04, 0x62, 0x6c, 0x61, 0x68
};

UNIT_TEST(JoinedSessionViewerTest);

TEST_CASE(JoinedSessionViewerTest, POST, [] () {
  Buffer test_session(test_session_bin, sizeof(test_session_bin));

  /*JoinedSession js;
  js.ParseFromArray(test_session.data(), test_session.size());

  fnord::iputs("has num cart items: $0", js.referrer_name());*/

  http::HTTPRequest request(http::HTTPMessage::M_POST, "/blah");
  request.addBody(test_session);
  http::HTTPResponse response;

  JoinedSessionViewer viewer;
  viewer.handleHTTPRequest(&request, &response);

  String session_param;
  util::Base64::encode(test_session.toString(), &session_param);

  EXPECT_EQ(response.statusCode(), 302);
  EXPECT_EQ(response.getHeader("Location"), "/view_session" + session_param);
});

TEST_CASE(JoinedSessionViewerTest, MissingParam, [] () {
  http::HTTPRequest request(http::HTTPMessage::M_GET, "/view_session");
  http::HTTPResponse response;

  JoinedSessionViewer viewer;
  viewer.handleHTTPRequest(&request, &response);

  EXPECT_EQ(response.statusCode(), 404);
});

TEST_CASE(JoinedSessionViewerTest, GET, [] () {
  http::HTTPRequest request(http::HTTPMessage::M_GET, "/view_session?session=ab");
  http::HTTPResponse response;

  JoinedSessionViewer viewer;
  viewer.handleHTTPRequest(&request, &response);

});
