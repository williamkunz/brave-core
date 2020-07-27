/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>

#include "base/test/task_environment.h"
#include "bat/ledger/internal/state/state_migration_v4.h"
#include "bat/ledger/internal/state/state_keys.h"
#include "bat/ledger/internal/ledger_client_mock.h"
#include "bat/ledger/internal/ledger_impl_mock.h"

// npm run test -- brave_unit_tests --filter=StateMigrationV4Test.*
using ::testing::_;
using ::testing::Invoke;

namespace braveledger_state {

class StateMigrationV4Test : public ::testing::Test {
 private:
  base::test::TaskEnvironment scoped_task_environment_;

 protected:
  std::unique_ptr<ledger::MockLedgerClient> mock_ledger_client_;
  std::unique_ptr<bat_ledger::MockLedgerImpl> mock_ledger_impl_;
  std::unique_ptr<StateMigrationV4> migration_;

  StateMigrationV4Test() {
      mock_ledger_client_ = std::make_unique<ledger::MockLedgerClient>();
      mock_ledger_impl_ = std::make_unique<bat_ledger::MockLedgerImpl>
          (mock_ledger_client_.get());
      migration_ = std::make_unique<StateMigrationV4>(mock_ledger_impl_.get());
  }
};

TEST_F(StateMigrationV4Test, EmptySeed) {
  // Call SetStringState
  ON_CALL(*mock_ledger_impl_, GetStringState(ledger::kStateRecoverySeed))
    .WillByDefault(testing::Return(""));

  EXPECT_CALL(*mock_ledger_impl_, SetStringState(_, _)).Times(0);
  migration_->Migrate([](const ledger::Result result) {
      ASSERT_EQ(result, ledger::Result::LEDGER_OK);
    });
}

TEST_F(StateMigrationV4Test, SeedIsEncrypted) {
  // Call SetStringState
  ON_CALL(*mock_ledger_impl_, GetStringState(ledger::kStateRecoverySeed))
    .WillByDefault(
        testing::Return("wYvZJy9yTiVrBJ1E+xfKmWj7rGndQpKgxwHBG+w4ojw="));

  const std::string expected_string =
      "djEwLSXuHL5rN6BnpSc6KqZqW5Qer202iny4G2NJfCn4gGQaKDxbexiBN7bUFVgedWp6";
  EXPECT_CALL(
      *mock_ledger_impl_,
      SetStringState(ledger::kStateRecoverySeed, expected_string))
    .Times(1);

  migration_->Migrate([](const ledger::Result result) {
      ASSERT_EQ(result, ledger::Result::LEDGER_OK);
    });
}

}  // namespace braveledger_state
