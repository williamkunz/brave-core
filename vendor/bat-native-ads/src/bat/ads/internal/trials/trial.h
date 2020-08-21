/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_TRIALS_TRIAL_H_
#define BAT_ADS_INTERNAL_TRIALS_TRIAL_H_

#include <stdint.h>

#include <string>

namespace ads {

class AdsImpl;

// TBD
class Trial {
 public:
  explicit Trial(
      AdsImpl* ads);

  ~Trial();

  uint16_t GetGroup();

  void Disable();

  uint16_t AppendGroup();

 private:
  // TODO(Moritz Haller): Retrieve from user prefs once @clifton is done and
  // figure out how to deal with defaults when user prefs doesn't have a group
  uint16_t group_ = 0;
  uint16_t default_group_ = 0;
  uint64_t expiry_date_in_seconds = 0;
  std::string name_;

  AdsImpl* ads_;  // NOT OWNED
};

// Factory pattern to create trials
class TrialList {

};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_TRIALS_TRIAL_H_
