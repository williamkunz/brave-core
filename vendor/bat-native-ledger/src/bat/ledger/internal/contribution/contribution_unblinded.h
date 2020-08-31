/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_CONTRIBUTION_CONTRIBUTION_UNBLINDED_H_
#define BRAVELEDGER_CONTRIBUTION_CONTRIBUTION_UNBLINDED_H_

#include <stdint.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "bat/ledger/internal/credentials/credentials_factory.h"
#include "bat/ledger/ledger.h"

namespace bat_ledger {
class LedgerImpl;
}

namespace braveledger_contribution {

using GetContributionInfoAndUnblindedTokensCallback = std::function<void(
    ledger::ContributionInfoPtr contribution,
    const std::vector<ledger::UnblindedToken>& list)>;

using Winners = std::map<std::string, uint32_t>;

class Unblinded {
 public:
  explicit Unblinded(bat_ledger::LedgerImpl* ledger);
  ~Unblinded();

  void Start(
      const std::vector<ledger::CredsBatchType>& types,
      const std::string& contribution_id,
      ledger::ResultCallback callback);

  void Retry(
      const std::vector<ledger::CredsBatchType>& types,
      ledger::ContributionInfoPtr contribution,
      ledger::ResultCallback callback);

 private:
  void GetContributionInfoAndUnblindedTokens(
      const std::vector<ledger::CredsBatchType>& types,
      const std::string& contribution_id,
      GetContributionInfoAndUnblindedTokensCallback callback);

  void OnUnblindedTokens(
      ledger::UnblindedTokenList list,
      const std::string& contribution_id,
      GetContributionInfoAndUnblindedTokensCallback callback);

  void GetContributionInfoAndReservedUnblindedTokens(
      const std::string& contribution_id,
      GetContributionInfoAndUnblindedTokensCallback callback);

  void OnReservedUnblindedTokens(
      ledger::UnblindedTokenList list,
      const std::string& contribution_id,
      GetContributionInfoAndUnblindedTokensCallback callback);

  void OnGetContributionInfo(
      ledger::ContributionInfoPtr contribution,
      const std::vector<ledger::UnblindedToken>& list,
      GetContributionInfoAndUnblindedTokensCallback callback);

  void PrepareTokens(
      ledger::ContributionInfoPtr contribution,
      const std::vector<ledger::UnblindedToken>& list,
      const std::vector<ledger::CredsBatchType>& types,
      ledger::ResultCallback callback);

  void PreparePublishers(
      const std::vector<ledger::UnblindedToken>& list,
      ledger::ContributionInfoPtr contribution,
      const std::vector<ledger::CredsBatchType>& types,
      ledger::ResultCallback callback);

  ledger::ContributionPublisherList PrepareAutoContribution(
      const std::vector<ledger::UnblindedToken>& list,
      ledger::ContributionInfoPtr contribution);

  void OnPrepareAutoContribution(
      const ledger::Result result,
      const std::vector<ledger::CredsBatchType>& types,
      const std::string& contribution_id,
      ledger::ResultCallback callback);

  void PrepareStepSaved(
      const ledger::Result result,
      const std::vector<ledger::CredsBatchType>& types,
      const std::string& contribution_id,
      ledger::ResultCallback callback);

  void ProcessTokens(
      const std::vector<ledger::CredsBatchType>& types,
      const std::string& contribution_id,
      ledger::ResultCallback callback);

  void OnProcessTokens(
      ledger::ContributionInfoPtr contribution,
      const std::vector<ledger::UnblindedToken>& list,
      ledger::ResultCallback callback);

  void TokenProcessed(
      const ledger::Result result,
      const std::string& contribution_id,
      const std::string& publisher_key,
      const bool final_publisher,
      ledger::ResultCallback callback);

  void ContributionAmountSaved(
      const ledger::Result result,
      const std::string& contribution_id,
      const bool final_publisher,
      ledger::ResultCallback callback);

  void OnMarkUnblindedTokensAsReserved(
      const ledger::Result result,
      const std::vector<ledger::UnblindedToken>& list,
      std::shared_ptr<ledger::ContributionInfoPtr> shared_contribution,
      const std::vector<ledger::CredsBatchType>& types,
      ledger::ResultCallback callback);

  void OnReservedUnblindedTokensForRetryAttempt(
      const ledger::UnblindedTokenList& list,
      const std::vector<ledger::CredsBatchType>& types,
      std::shared_ptr<ledger::ContributionInfoPtr> shared_contribution,
      ledger::ResultCallback callback);

  bat_ledger::LedgerImpl* ledger_;  // NOT OWNED
  std::unique_ptr<braveledger_credentials::Credentials> credentials_promotion_;
  std::unique_ptr<braveledger_credentials::Credentials> credentials_sku_;
};

}  // namespace braveledger_contribution
#endif  // BRAVELEDGER_CONTRIBUTION_CONTRIBUTION_UNBLINDED_H_
