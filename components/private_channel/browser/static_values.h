  /* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_PRIVATE_CHANNEL_BROWSER_STATIC_VALUES_H_
#define BRAVE_COMPONENTS_PRIVATE_CHANNEL_BROWSER_STATIC_VALUES_H_

#include <stdint.h>
#include <string>

const int kMaxPrivateChannelServerResponseSizeBytes = 1024 * 1024;

#define PRIVATE_CHANNEL_STAGING_SERVER     \
"https://repsys.rewards.brave.software"
#define PRIVATE_CHANNEL_PRODUCTION_SERVER   \
"https://repsys.rewards.brave.com"
#define PRIVATE_CHANNEL_DEVELOPMENT_SERVER   \
"https://repsys.rewards.brave.software"

#define PRIVATE_CHANNEL_API_VERSION "/public/v1"
#define PRIVATE_CHANNEL_META_ENDPOINT "/private-channel/meta"
#define PRIVATE_CHANNEL_FIRST_ROUND_ENDPOINT "/private-channel/start"
#define PRIVATE_CHANNEL_SECOND_ROUND_ENDPOINT "/private-channel/result"

static const uint8_t PRIVATE_CHANNEL_SERVER_PK[] = {
  216, 8, 125, 12, 194, 66, 33, 188, 65, 82, 9, 38, 140, 163,
  231, 156, 2, 3, 82, 85, 115, 97, 14, 54, 85, 62, 0, 11, 213,
  142, 152, 1};

#endif  // BRAVE_COMPONENTS_PRIVATE_CHANNEL_BROWSER_STATIC_VALUES_H_
