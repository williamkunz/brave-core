/* Copyright 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "private_channel.hpp"
#include <iostream>

extern "C" {
#include "lib.h"  // NOLINT
}

namespace private_channel {

  ResultChallenge start_challenge(
    const char* const* input_ptr, int size, const uint8_t* server_pk) {
      return client_start_challenge(input_ptr, size, server_pk);
  }

  ResultSecondRound second_round(
    const uint8_t* enc_input_ptr, int input_size, const uint8_t*  sk) {
      return client_second_round(enc_input_ptr, input_size, sk);
  }

  void free_first_round_result(ResultChallenge result) {
    return deallocate_first_round_result(result);
  }

  void free_second_round_result(ResultSecondRound result) {
    return deallocate_second_round_result(result);
  }

}  // namespace private_channel
