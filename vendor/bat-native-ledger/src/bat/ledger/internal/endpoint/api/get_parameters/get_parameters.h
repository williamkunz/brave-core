/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_ENDPOINT_API_GET_PARAMETERS_GET_PARAMETERS_H_
#define BRAVELEDGER_ENDPOINT_API_GET_PARAMETERS_GET_PARAMETERS_H_

#include <string>

#include "bat/ledger/ledger.h"

// GET /v1/parameters
// GET /v1/parameters?currency={currency}
//
// Success code:
// HTTP_OK (200)
//
// Error codes:
// HTTP_BAD_REQUEST (400)
// HTTP_INTERNAL_SERVER_ERROR (500)
//
// Response body:
// {
//   "batRate": 0.2476573499489187,
//   "autocontribute": {
//     "choices": [
//       5,
//       10,
//       15,
//       20,
//       25,
//       50,
//       100
//     ],
//     "defaultChoice": 20
//   },
//   "tips": {
//     "defaultTipChoices": [
//       1,
//       10,
//       100
//     ],
//     "defaultMonthlyChoices": [
//       1,
//       10,
//       100
//     ]
//   }
// }

namespace bat_ledger {
class LedgerImpl;
}

namespace ledger {
namespace endpoint {
namespace api {

using GetParametersCallback = std::function<void(
    const ledger::Result result,
    const ledger::RewardsParameters&)>;

class GetParameters {
 public:
  explicit GetParameters(bat_ledger::LedgerImpl* ledger);
  ~GetParameters();

  void Request(GetParametersCallback callback);

 private:
  std::string GetUrl(const std::string& currency = "");

  ledger::Result CheckStatusCode(const int status_code);

  ledger::Result ParseBody(
      const std::string& body,
      ledger::RewardsParameters* parameters);

  void OnRequest(
      const ledger::UrlResponse& response,
      GetParametersCallback callback);

  bat_ledger::LedgerImpl* ledger_;  // NOT OWNED
};

}  // namespace api
}  // namespace endpoint
}  // namespace ledger

#endif  // BRAVELEDGER_ENDPOINT_API_GET_PARAMETERS_GET_PARAMETERS_H_
