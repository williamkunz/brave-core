/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_SKU_MERCHANT_H_
#define BRAVELEDGER_SKU_MERCHANT_H_

#include <memory>
#include <string>
#include <vector>

#include "bat/ledger/internal/sku/sku.h"
#include "bat/ledger/internal/sku/sku_common.h"
#include "bat/ledger/ledger.h"

namespace bat_ledger {
class LedgerImpl;
}

namespace braveledger_sku {

class SKUMerchant : public SKU  {
 public:
  explicit SKUMerchant(bat_ledger::LedgerImpl* ledger);
  ~SKUMerchant() override;

  void Process(
      const std::vector<ledger::SKUOrderItem>& items,
      ledger::ExternalWalletPtr wallet,
      ledger::SKUOrderCallback callback,
      const std::string& contribution_id = "") override;

  void Retry(
      const std::string& order_id,
      ledger::ExternalWalletPtr wallet,
      ledger::SKUOrderCallback callback) override;

 private:
  void OrderCreated(
      const ledger::Result result,
      const std::string& order_id,
      const ledger::ExternalWallet& wallet,
      ledger::SKUOrderCallback callback);

  void OnOrder(
      ledger::SKUOrderPtr order,
      const ledger::ExternalWallet& wallet,
      ledger::SKUOrderCallback callback);

  void OnServerPublisherInfo(
      ledger::ServerPublisherInfoPtr info,
      std::shared_ptr<ledger::SKUOrderPtr> shared_order,
      const ledger::ExternalWallet& wallet,
      ledger::SKUOrderCallback callback);

  bat_ledger::LedgerImpl* ledger_;  // NOT OWNED
  std::unique_ptr<SKUCommon> common_;
};

}  // namespace braveledger_sku

#endif  // BRAVELEDGER_SKU_MERCHANT_H_
