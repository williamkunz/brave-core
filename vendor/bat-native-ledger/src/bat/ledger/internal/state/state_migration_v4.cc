/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>

#include "base/base64.h"
#include "bat/ledger/internal/ledger_impl.h"
#include "bat/ledger/internal/state/state_keys.h"
#include "bat/ledger/internal/state/state_migration_v4.h"
#include "bat/ledger/internal/state/state_util.h"
#include "components/os_crypt/os_crypt.h"

namespace braveledger_state {

StateMigrationV4::StateMigrationV4(bat_ledger::LedgerImpl* ledger) :
    ledger_(ledger) {
}

StateMigrationV4::~StateMigrationV4() = default;

void StateMigrationV4::Migrate(ledger::ResultCallback callback) {
  const std::string encoded_seed =
      ledger_->GetStringState(ledger::kStateRecoverySeed);

  if (encoded_seed.empty()) {
    BLOG(1, "Seed is empty");
    callback(ledger::Result::LEDGER_OK);
    return;
  }

  std::string encrypted_seed;
  if (!OSCrypt::EncryptString(encoded_seed, &encrypted_seed)) {
    BLOG(0, "Encrypting seed failed");
    callback(ledger::Result::LEDGER_ERROR);
    return;
  }

  std::string new_seed;
  base::Base64Encode(encrypted_seed, &new_seed);

  if (new_seed.empty()) {
    BLOG(0, "Encoding seed failed");
    callback(ledger::Result::LEDGER_ERROR);
    return;
  }

  ledger_->SetStringState(ledger::kStateRecoverySeed, new_seed);
  callback(ledger::Result::LEDGER_OK);
}

}  // namespace braveledger_state
