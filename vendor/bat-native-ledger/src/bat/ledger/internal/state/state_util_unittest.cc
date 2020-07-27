/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/state/state_util.h"
#include "testing/gtest/include/gtest/gtest.h"

// npm run test -- brave_unit_tests --filter=StateUtilTest.*

namespace braveledger_state {

class StateUtilTest : public testing::Test {
};

TEST(StateUtilTest, EncryptingString) {
  const std::string password = "fpoaskdfoiasjdf0n3409cm39324-cj3f=21123rerger";
  std::string encrypted_string;
  std::string decrypted_string;
  EXPECT_TRUE(EncryptString(password, &encrypted_string));
  EXPECT_TRUE(DecryptString(encrypted_string, &decrypted_string));
  EXPECT_EQ(password, decrypted_string);
}

}  // namespace braveledger_state
