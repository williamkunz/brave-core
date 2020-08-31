/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_PROMOTION_H_
#define BRAVELEDGER_PROMOTION_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "base/timer/timer.h"
#include "bat/ledger/ledger.h"
#include "bat/ledger/mojom_structs.h"
#include "bat/ledger/internal/attestation/attestation_impl.h"
#include "bat/ledger/internal/credentials/credentials_factory.h"
#include "bat/ledger/internal/endpoint/promotion/promotion_server.h"

namespace bat_ledger {
class LedgerImpl;
}

namespace braveledger_promotion {

class PromotionTransfer;

class Promotion {
 public:
  explicit Promotion(bat_ledger::LedgerImpl* ledger);
  ~Promotion();

  void Initialize();

  void Fetch(ledger::FetchPromotionCallback callback);

  void Claim(
      const std::string& promotion_id,
      const std::string& payload,
      ledger::ClaimPromotionCallback callback);

  void Attest(
      const std::string& promotion_id,
      const std::string& solution,
      ledger::AttestPromotionCallback callback);

  void Refresh(const bool retry_after_error);

  void TransferTokens(ledger::ResultCallback callback);

 private:
  void OnFetch(
      const ledger::Result result,
      ledger::PromotionList list,
      const std::vector<std::string>& corrupted_promotions,
      ledger::FetchPromotionCallback callback);

  void OnGetAllPromotions(
      ledger::PromotionMap promotions,
      std::shared_ptr<ledger::PromotionList> list,
      ledger::FetchPromotionCallback callback);

  void OnGetAllPromotionsFromDatabase(
      ledger::PromotionMap promotions,
      ledger::FetchPromotionCallback callback);

  void LegacyClaimedSaved(
      const ledger::Result result,
      std::shared_ptr<ledger::PromotionPtr> shared_promotion);

  void OnClaimPromotion(
      ledger::PromotionPtr promotion,
      const std::string& payload,
      ledger::ClaimPromotionCallback callback);

  void OnAttestPromotion(
      ledger::PromotionPtr promotion,
      const std::string& solution,
      ledger::AttestPromotionCallback callback);

  void OnAttestedPromotion(
      const ledger::Result result,
      const std::string& promotion_id,
      ledger::AttestPromotionCallback callback);

  void OnCompletedAttestation(
      ledger::PromotionPtr promotion,
      ledger::AttestPromotionCallback callback);

  void AttestedSaved(
      const ledger::Result result,
      std::shared_ptr<ledger::PromotionPtr> shared_promotion,
      ledger::AttestPromotionCallback callback);

  void Complete(
      const ledger::Result result,
      const std::string& promotion_string,
      ledger::AttestPromotionCallback callback);

  void OnComplete(
      ledger::PromotionPtr promotion,
      const ledger::Result result,
      ledger::AttestPromotionCallback callback);

  void ProcessFetchedPromotions(
      const ledger::Result result,
      ledger::PromotionList promotions,
      ledger::FetchPromotionCallback callback);

  void GetCredentials(
      ledger::PromotionPtr promotion,
      ledger::ResultCallback callback);

  void CredentialsProcessed(
      const ledger::Result result,
      const std::string& promotion_id,
      ledger::ResultCallback callback);

  void Retry(ledger::PromotionMap promotions);

  void CheckForCorrupted(const ledger::PromotionMap& promotions);

  void CorruptedPromotionFixed(const ledger::Result result);

  void CheckForCorruptedCreds(ledger::CredsBatchList list);

  void CorruptedPromotions(
      ledger::PromotionList promotions,
      const std::vector<std::string>& ids);

  void OnCheckForCorrupted(
      const ledger::Result result,
      const std::vector<std::string>& promotion_id_list);

  void ErrorStatusSaved(
      const ledger::Result result,
      const std::vector<std::string>& promotion_id_list);

  void ErrorCredsStatusSaved(const ledger::Result result);

  void OnRetryTimerElapsed();

  void OnLastCheckTimerElapsed();

  std::unique_ptr<ledger::attestation::AttestationImpl> attestation_;
  std::unique_ptr<PromotionTransfer> transfer_;
  std::unique_ptr<braveledger_credentials::Credentials> credentials_;
  std::unique_ptr<ledger::endpoint::PromotionServer> promotion_server_;
  bat_ledger::LedgerImpl* ledger_;  // NOT OWNED
  base::OneShotTimer last_check_timer_;
  base::OneShotTimer retry_timer_;
};

}  // namespace braveledger_promotion

#endif  // BRAVELEDGER_PROMOTION_H_
