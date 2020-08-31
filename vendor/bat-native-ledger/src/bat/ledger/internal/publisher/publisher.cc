/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <algorithm>
#include <cmath>
#include <ctime>
#include <map>
#include <utility>
#include <vector>

#include "base/guid.h"
#include "bat/ledger/global_constants.h"
#include "bat/ledger/internal/ledger_impl.h"
#include "bat/ledger/internal/publisher/prefix_util.h"
#include "bat/ledger/internal/publisher/publisher.h"
#include "bat/ledger/internal/publisher/publisher_prefix_list_updater.h"
#include "bat/ledger/internal/publisher/server_publisher_fetcher.h"
#include "bat/ledger/internal/static_values.h"

using std::placeholders::_1;
using std::placeholders::_2;

namespace braveledger_publisher {

Publisher::Publisher(bat_ledger::LedgerImpl* ledger):
    ledger_(ledger),
    prefix_list_updater_(
        std::make_unique<PublisherPrefixListUpdater>(ledger)),
    server_publisher_fetcher_(
        std::make_unique<ServerPublisherFetcher>(ledger)) {
}

Publisher::~Publisher() = default;

bool Publisher::ShouldFetchServerPublisherInfo(
    ledger::ServerPublisherInfo* server_info) {
  return server_publisher_fetcher_->IsExpired(server_info);
}

void Publisher::FetchServerPublisherInfo(
    const std::string& publisher_key,
    ledger::GetServerPublisherInfoCallback callback) {
  server_publisher_fetcher_->Fetch(publisher_key, callback);
}

void Publisher::RefreshPublisher(
    const std::string& publisher_key,
    ledger::OnRefreshPublisherCallback callback) {
  // Bypass cache and unconditionally fetch the latest info
  // for the specified publisher.
  server_publisher_fetcher_->Fetch(publisher_key,
      [this, callback](auto server_info) {
        auto status = server_info
            ? server_info->status
            : ledger::PublisherStatus::NOT_VERIFIED;

        // If, after refresh, the publisher is now verified
        // attempt to process any pending contributions for
        // unverified publishers.
        if (status == ledger::PublisherStatus::VERIFIED) {
          ledger_->contribution()->ContributeUnverifiedPublishers();
        }

        callback(status);
      });
}

void Publisher::SetPublisherServerListTimer() {
  if (ledger_->state()->GetRewardsMainEnabled()) {
    prefix_list_updater_->StartAutoUpdate([this]() {
      // Attempt to reprocess any contributions for previously
      // unverified publishers that are now verified.
      ledger_->contribution()->ContributeUnverifiedPublishers();
    });
  } else {
    prefix_list_updater_->StopAutoUpdate();
  }
}

void Publisher::CalcScoreConsts(const int min_duration_seconds) {
  // we increase duration for 100 to keep it as close to muon implementation
  // as possible (we used 1000 in muon)
  // keeping it with only seconds visits are not spaced out equally
  uint64_t min_duration_big = min_duration_seconds * 100;
  const double d = 1.0 / (30.0 * 1000.0);
  const double a = (1.0 / (d * 2.0)) - min_duration_big;
  const double b = min_duration_big - a;

  ledger_->state()->SetScoreValues(a, b);
}

// courtesy of @dimitry-xyz:
// https://github.com/brave/ledger/issues/2#issuecomment-221752002
double Publisher::concaveScore(const uint64_t& duration_seconds) {
  uint64_t duration_big = duration_seconds * 100;
  double a, b;
  ledger_->state()->GetScoreValues(&a, &b);
  return (-b + std::sqrt((b * b) + (a * 4 * duration_big))) / (a * 2);
}

std::string getProviderName(const std::string& publisher_id) {
  // TODO(anyone) this is for the media stuff
  if (publisher_id.find(YOUTUBE_MEDIA_TYPE) != std::string::npos) {
    return YOUTUBE_MEDIA_TYPE;
  } else if (publisher_id.find(TWITCH_MEDIA_TYPE) != std::string::npos) {
    return TWITCH_MEDIA_TYPE;
  } else if (publisher_id.find(TWITTER_MEDIA_TYPE) != std::string::npos) {
    return TWITTER_MEDIA_TYPE;
  } else if (publisher_id.find(VIMEO_MEDIA_TYPE) != std::string::npos) {
    return VIMEO_MEDIA_TYPE;
  } else if (publisher_id.find(GITHUB_MEDIA_TYPE) != std::string::npos) {
    return GITHUB_MEDIA_TYPE;
  }

  return "";
}

bool ignoreMinTime(const std::string& publisher_id) {
  return !getProviderName(publisher_id).empty();
}

void Publisher::SaveVisit(
    const std::string& publisher_key,
    const ledger::VisitData& visit_data,
    const uint64_t& duration,
    uint64_t window_id,
    const ledger::PublisherInfoCallback callback) {
  if (!ledger_->state()->GetRewardsMainEnabled()) {
    return;
  }

  if (publisher_key.empty()) {
    BLOG(0, "Publisher key is empty");
    return;
  }

  auto on_server_info = std::bind(&Publisher::OnSaveVisitServerPublisher,
      this, _1, publisher_key, visit_data, duration, window_id, callback);

  ledger_->database()->SearchPublisherPrefixList(
      publisher_key,
      [this, publisher_key, on_server_info](bool publisher_exists) {
        if (publisher_exists) {
          GetServerPublisherInfo(publisher_key, on_server_info);
        } else {
          on_server_info(nullptr);
        }
      });
}

void Publisher::SaveVideoVisit(
    const std::string& publisher_id,
    const ledger::VisitData& visit_data,
    uint64_t duration,
    uint64_t window_id,
    ledger::PublisherInfoCallback callback) {
  if (!ledger_->state()->GetPublisherAllowVideos()) {
    duration = 0;
  }

  SaveVisit(publisher_id, visit_data, duration, window_id, callback);
}

ledger::ActivityInfoFilterPtr Publisher::CreateActivityFilter(
    const std::string& publisher_id,
    ledger::ExcludeFilter excluded,
    bool min_duration,
    const uint64_t& current_reconcile_stamp,
    bool non_verified,
    bool min_visits) {
  auto filter = ledger::ActivityInfoFilter::New();
  filter->id = publisher_id;
  filter->excluded = excluded;
  filter->min_duration = min_duration
      ? ledger_->state()->GetPublisherMinVisitTime()
      : 0;
  filter->reconcile_stamp = current_reconcile_stamp;
  filter->non_verified = non_verified;
  filter->min_visits = min_visits
      ? ledger_->state()->GetPublisherMinVisits()
      : 0;

  return filter;
}

void Publisher::OnSaveVisitServerPublisher(
    ledger::ServerPublisherInfoPtr server_info,
    const std::string& publisher_key,
    const ledger::VisitData& visit_data,
    uint64_t duration,
    uint64_t window_id,
    const ledger::PublisherInfoCallback callback) {
  auto filter = CreateActivityFilter(
      publisher_key,
      ledger::ExcludeFilter::FILTER_ALL,
      false,
      ledger_->state()->GetReconcileStamp(),
      true,
      false);

  // we need to do this as I can't move server publisher into final function
  auto status = ledger::PublisherStatus::NOT_VERIFIED;
  if (server_info) {
    status = server_info->status;
  }

  ledger::PublisherInfoCallback get_callback =
      std::bind(&Publisher::SaveVisitInternal,
          this,
          status,
          publisher_key,
          visit_data,
          duration,
          window_id,
          callback,
          _1,
          _2);

  auto list_callback = std::bind(&Publisher::OnGetActivityInfo,
      this,
      _1,
      get_callback,
      filter->id);

  ledger_->database()->GetActivityInfoList(
      0,
      2,
      std::move(filter),
      list_callback);
}

void Publisher::OnGetActivityInfo(
    ledger::PublisherInfoList list,
    ledger::PublisherInfoCallback callback,
    const std::string& publisher_key) {
  if (list.empty()) {
    ledger_->database()->GetPublisherInfo(publisher_key, callback);
    return;
  }

  if (list.size() > 1) {
    callback(ledger::Result::TOO_MANY_RESULTS, nullptr);
    return;
  }

  callback(ledger::Result::LEDGER_OK, std::move(list[0]));
}

void Publisher::SaveVisitInternal(
    const ledger::PublisherStatus status,
    const std::string& publisher_key,
    const ledger::VisitData& visit_data,
    uint64_t duration,
    uint64_t window_id,
    const ledger::PublisherInfoCallback callback,
    ledger::Result result,
    ledger::PublisherInfoPtr publisher_info) {
  DCHECK(result != ledger::Result::TOO_MANY_RESULTS);
  if (result != ledger::Result::LEDGER_OK &&
      result != ledger::Result::NOT_FOUND) {
    BLOG(0, "Visit was not saved " << result);
    callback(ledger::Result::LEDGER_ERROR, nullptr);
    return;
  }

  bool is_verified = IsConnectedOrVerified(status);

  bool new_visit = false;
  if (!publisher_info) {
    new_visit = true;
    publisher_info = ledger::PublisherInfo::New();
    publisher_info->id = publisher_key;
  }

  std::string fav_icon = visit_data.favicon_url;
  if (is_verified && !fav_icon.empty()) {
    if (fav_icon.find(".invalid") == std::string::npos) {
    ledger_->ledger_client()->FetchFavIcon(
        fav_icon,
        "https://" + base::GenerateGUID() + ".invalid",
        std::bind(&Publisher::onFetchFavIcon,
            this,
            publisher_info->id,
            window_id,
            _1,
            _2));
    } else {
        publisher_info->favicon_url = fav_icon;
    }
  } else {
    publisher_info->favicon_url = ledger::kClearFavicon;
  }

  publisher_info->name = visit_data.name;
  publisher_info->provider = visit_data.provider;
  publisher_info->url = visit_data.url;
  publisher_info->status = status;

  bool excluded =
      publisher_info->excluded == ledger::PublisherExclude::EXCLUDED;
  bool ignore_time = ignoreMinTime(publisher_key);
  if (duration == 0) {
    ignore_time = false;
  }

  ledger::PublisherInfoPtr panel_info = nullptr;

  uint64_t min_visit_time = static_cast<uint64_t>(
      ledger_->state()->GetPublisherMinVisitTime());

  // for new visits that are excluded or are not long enough or ac is off
  bool allow_non_verified = ledger_->state()->GetPublisherAllowNonVerified();
  bool min_duration_new = duration < min_visit_time && !ignore_time;
  bool min_duration_ok = duration > min_visit_time || ignore_time;
  bool verified_new = !allow_non_verified && !is_verified;
  bool verified_old = allow_non_verified || is_verified;

  if (new_visit &&
      (excluded ||
       !ledger_->state()->GetAutoContributeEnabled() ||
       min_duration_new ||
       verified_new)) {
    panel_info = publisher_info->Clone();

    auto callback = std::bind(&Publisher::OnPublisherInfoSaved,
        this,
        _1);

    ledger_->database()->SavePublisherInfo(publisher_info->Clone(), callback);
    ledger_->database()->SaveActivityInfo(std::move(publisher_info), callback);
  } else if (!excluded &&
             ledger_->state()->GetAutoContributeEnabled() &&
             min_duration_ok &&
             verified_old) {
    publisher_info->visits += 1;
    publisher_info->duration += duration;
    publisher_info->score += concaveScore(duration);
    publisher_info->reconcile_stamp = ledger_->state()->GetReconcileStamp();

    panel_info = publisher_info->Clone();

    auto callback = std::bind(&Publisher::OnPublisherInfoSaved,
        this,
        _1);

    ledger_->database()->SaveActivityInfo(std::move(publisher_info), callback);
  }

  if (panel_info) {
    if (panel_info->favicon_url == ledger::kClearFavicon) {
      panel_info->favicon_url = std::string();
    }

    auto callback_info = panel_info->Clone();
    callback(ledger::Result::LEDGER_OK, std::move(callback_info));

    if (window_id > 0) {
      OnPanelPublisherInfo(ledger::Result::LEDGER_OK,
                           std::move(panel_info),
                           window_id,
                           visit_data);
    }
  }
}

void Publisher::onFetchFavIcon(const std::string& publisher_key,
                                   uint64_t window_id,
                                   bool success,
                                   const std::string& favicon_url) {
  if (!success || favicon_url.empty()) {
    BLOG(1, "Corrupted favicon file");
    return;
  }

  ledger_->database()->GetPublisherInfo(publisher_key,
      std::bind(&Publisher::onFetchFavIconDBResponse,
                this,
                _1,
                _2,
                favicon_url,
                window_id));
}

void Publisher::onFetchFavIconDBResponse(
    ledger::Result result,
    ledger::PublisherInfoPtr info,
    const std::string& favicon_url,
    uint64_t window_id) {
  if (result != ledger::Result::LEDGER_OK || favicon_url.empty()) {
    BLOG(1, "Missing or corrupted favicon file");
    return;
  }

  info->favicon_url = favicon_url;

  auto callback = std::bind(&Publisher::OnPublisherInfoSaved,
      this,
      _1);

  ledger_->database()->SavePublisherInfo(info->Clone(), callback);

  if (window_id > 0) {
    ledger::VisitData visit_data;
    OnPanelPublisherInfo(ledger::Result::LEDGER_OK,
                        std::move(info),
                        window_id,
                        visit_data);
  }
}

void Publisher::OnPublisherInfoSaved(const ledger::Result result) {
  if (result != ledger::Result::LEDGER_OK) {
    BLOG(0, "Publisher info was not saved!");
    return;
  }

  SynopsisNormalizer();
}

void Publisher::SetPublisherExclude(
    const std::string& publisher_id,
    const ledger::PublisherExclude& exclude,
    ledger::ResultCallback callback) {
  ledger_->database()->GetPublisherInfo(
      publisher_id,
      std::bind(&Publisher::OnSetPublisherExclude,
                this,
                exclude,
                _1,
                _2,
                callback));
}

void Publisher::OnSetPublisherExclude(
    ledger::PublisherExclude exclude,
    ledger::Result result,
    ledger::PublisherInfoPtr publisher_info,
    ledger::ResultCallback callback) {
  if (result != ledger::Result::LEDGER_OK &&
      result != ledger::Result::NOT_FOUND) {
    BLOG(0, "Publisher exclude status not saved");
    callback(result);
    return;
  }

  if (!publisher_info) {
    BLOG(0, "Publisher is null");
    callback(ledger::Result::LEDGER_ERROR);
    return;
  }

  if (publisher_info->excluded == exclude) {
    callback(ledger::Result::LEDGER_OK);
    return;
  }

  publisher_info->excluded = exclude;

  auto save_callback = std::bind(&Publisher::OnPublisherInfoSaved,
      this,
      _1);
  ledger_->database()->SavePublisherInfo(
      publisher_info->Clone(),
      save_callback);
  if (exclude == ledger::PublisherExclude::EXCLUDED) {
    ledger_->database()->DeleteActivityInfo(
      publisher_info->id,
      [](const ledger::Result _){});
  }
  callback(ledger::Result::LEDGER_OK);
}

void Publisher::OnRestorePublishers(
    const ledger::Result result,
    ledger::ResultCallback callback) {
  if (result != ledger::Result::LEDGER_OK) {
    BLOG(0, "Could not restore publishers.");
    callback(result);
    return;
  }

  SynopsisNormalizer();
  callback(ledger::Result::LEDGER_OK);
}

void Publisher::NormalizeContributeWinners(
    ledger::PublisherInfoList* newList,
    const ledger::PublisherInfoList* list,
    uint32_t record) {
  synopsisNormalizerInternal(newList, list, record);
}

void Publisher::synopsisNormalizerInternal(
    ledger::PublisherInfoList* newList,
    const ledger::PublisherInfoList* list,
    uint32_t /* next_record */) {
  if (list->empty()) {
    BLOG(1, "Publisher list is empty");
    return;
  }

  double totalScores = 0.0;
  for (size_t i = 0; i < list->size(); i++) {
    totalScores += (*list)[i]->score;
  }

  std::vector<unsigned int> percents;
  std::vector<double> weights;
  std::vector<double> realPercents;
  std::vector<double> roundoffs;
  unsigned int totalPercents = 0;
  for (size_t i = 0; i < list->size(); i++) {
    double floatNumber = ((*list)[i]->score / totalScores) * 100.0;
    double roundNumber = (unsigned int)std::lround(floatNumber);
    realPercents.push_back(floatNumber);
    percents.push_back(roundNumber);
    double roundoff = roundNumber - floatNumber;
    if (roundoff < 0.0) {
      roundoff *= -1.0;
    }
    roundoffs.push_back(roundoff);
    totalPercents += roundNumber;
    weights.push_back(floatNumber);
  }
  while (totalPercents != 100) {
    size_t valueToChange = 0;
    double currentRoundOff = 0.0;
    for (size_t i = 0; i < percents.size(); i++) {
      if (i == 0) {
        currentRoundOff = roundoffs[i];
        continue;
      }
      if (roundoffs[i] > currentRoundOff) {
        currentRoundOff = roundoffs[i];
        valueToChange = i;
      }
    }
    if (percents.size() != 0) {
      if (totalPercents > 100) {
        if (percents[valueToChange] != 0) {
          percents[valueToChange] -= 1;
          totalPercents -= 1;
        }
      } else {
        if (percents[valueToChange] != 100) {
          percents[valueToChange] += 1;
          totalPercents += 1;
        }
      }
      roundoffs[valueToChange] = 0;
    }
  }
  size_t currentValue = 0;
  for (size_t i = 0; i < list->size(); i++) {
    (*list)[i]->percent = percents[currentValue];
    (*list)[i]->weight = weights[currentValue];
    currentValue++;
    if (newList) {
      newList->push_back((*list)[i]->Clone());
    }
  }
}

void Publisher::SynopsisNormalizer() {
  auto filter = CreateActivityFilter("",
      ledger::ExcludeFilter::FILTER_ALL_EXCEPT_EXCLUDED,
      true,
      ledger_->state()->GetReconcileStamp(),
      ledger_->state()->GetPublisherAllowNonVerified(),
      ledger_->state()->GetPublisherMinVisits());
  ledger_->database()->GetActivityInfoList(
      0,
      0,
      std::move(filter),
      std::bind(&Publisher::SynopsisNormalizerCallback, this, _1));
}

void Publisher::SynopsisNormalizerCallback(
    ledger::PublisherInfoList list) {
  ledger::PublisherInfoList normalized_list;
  synopsisNormalizerInternal(&normalized_list, &list, 0);
  ledger::PublisherInfoList save_list;
  for (auto& item : list) {
    save_list.push_back(item.Clone());
  }

  ledger_->database()->NormalizeActivityInfoList(
      std::move(save_list),
      [](const ledger::Result){});
}

bool Publisher::IsConnectedOrVerified(const ledger::PublisherStatus status) {
  return status == ledger::PublisherStatus::CONNECTED ||
         status == ledger::PublisherStatus::VERIFIED;
}

void Publisher::GetPublisherActivityFromUrl(
    uint64_t windowId,
    ledger::VisitDataPtr visit_data,
    const std::string& publisher_blob) {
  if (!ledger_->state()->GetRewardsMainEnabled() || !visit_data) {
    return;
  }

  const bool is_media =
      visit_data->domain == YOUTUBE_TLD ||
      visit_data->domain == TWITCH_TLD ||
      visit_data->domain == TWITTER_TLD ||
      visit_data->domain == REDDIT_TLD ||
      visit_data->domain == VIMEO_TLD ||
      visit_data->domain == GITHUB_TLD;

  if (is_media && visit_data->path != "" && visit_data->path != "/") {
    std::string type = YOUTUBE_MEDIA_TYPE;
    if (visit_data->domain == TWITCH_TLD) {
      type = TWITCH_MEDIA_TYPE;
    } else if (visit_data->domain == TWITTER_TLD) {
      type = TWITTER_MEDIA_TYPE;
    } else if (visit_data->domain == REDDIT_TLD) {
      type = REDDIT_MEDIA_TYPE;
    } else if (visit_data->domain == VIMEO_TLD) {
      type = VIMEO_MEDIA_TYPE;
    } else if (visit_data->domain == GITHUB_TLD) {
      type = GITHUB_MEDIA_TYPE;
    }

    if (!visit_data->url.empty()) {
      visit_data->url.pop_back();
    }

    visit_data->url += visit_data->path;

    ledger_->media()->GetMediaActivityFromUrl(
        windowId,
        std::move(visit_data),
        type,
        publisher_blob);
    return;
  }

  auto filter = CreateActivityFilter(
      visit_data->domain,
      ledger::ExcludeFilter::FILTER_ALL,
      false,
      ledger_->state()->GetReconcileStamp(),
      true,
      false);

  visit_data->favicon_url = "";

  ledger_->database()->GetPanelPublisherInfo(
      std::move(filter),
      std::bind(&Publisher::OnPanelPublisherInfo,
          this,
          _1,
          _2,
          windowId,
          *visit_data));
}

void Publisher::OnSaveVisitInternal(
    ledger::Result result,
    ledger::PublisherInfoPtr info) {
  // TODO(nejczdovc): handle if needed
}

void Publisher::OnPanelPublisherInfo(
    ledger::Result result,
    ledger::PublisherInfoPtr info,
    uint64_t windowId,
    const ledger::VisitData& visit_data) {
  if (result == ledger::Result::LEDGER_OK) {
    ledger_->ledger_client()->OnPanelPublisherInfo(
        result,
        std::move(info),
        windowId);
    return;
  }

  if (result == ledger::Result::NOT_FOUND && !visit_data.domain.empty()) {
    auto callback = std::bind(&Publisher::OnSaveVisitInternal,
                              this,
                              _1,
                              _2);

    SaveVisit(visit_data.domain, visit_data, 0, windowId, callback);
  }
}

void Publisher::GetPublisherBanner(
    const std::string& publisher_key,
    ledger::PublisherBannerCallback callback) {
  const auto banner_callback = std::bind(&Publisher::OnGetPublisherBanner,
                this,
                _1,
                publisher_key,
                callback);

  // NOTE: We do not attempt to search the prefix list before getting
  // the publisher data because if the prefix list was not properly
  // loaded then the user would not see the correct banner information
  // for a verified publisher. Assuming that the user has explicitly
  // requested this information by interacting with the UI, we should
  // make a best effort to return correct and updated information even
  // if the prefix list is incorrect.
  GetServerPublisherInfo(publisher_key, banner_callback);
}

void Publisher::OnGetPublisherBanner(
    ledger::ServerPublisherInfoPtr info,
    const std::string& publisher_key,
    ledger::PublisherBannerCallback callback) {
  auto banner = ledger::PublisherBanner::New();

  if (info) {
    if (info->banner) {
      banner = info->banner->Clone();
    }

    banner->status = info->status;
  }

  banner->publisher_key = publisher_key;

  const auto publisher_callback =
      std::bind(&Publisher::OnGetPublisherBannerPublisher,
                this,
                callback,
                *banner,
                _1,
                _2);

  ledger_->database()->GetPublisherInfo(publisher_key, publisher_callback);
}

void Publisher::OnGetPublisherBannerPublisher(
    ledger::PublisherBannerCallback callback,
    const ledger::PublisherBanner& banner,
    ledger::Result result,
    ledger::PublisherInfoPtr publisher_info) {
  auto new_banner = ledger::PublisherBanner::New(banner);

  if (!publisher_info || result != ledger::Result::LEDGER_OK) {
    BLOG(0, "Publisher info not found");
    callback(std::move(new_banner));
    return;
  }

  new_banner->name = publisher_info->name;
  new_banner->provider = publisher_info->provider;

  if (new_banner->logo.empty()) {
    new_banner->logo = publisher_info->favicon_url;
  }

  callback(std::move(new_banner));
}

void Publisher::GetServerPublisherInfo(
    const std::string& publisher_key,
    ledger::GetServerPublisherInfoCallback callback) {
  ledger_->database()->GetServerPublisherInfo(
      publisher_key,
      std::bind(&Publisher::OnServerPublisherInfoLoaded,
          this,
          _1,
          publisher_key,
          callback));
}

void Publisher::OnServerPublisherInfoLoaded(
    ledger::ServerPublisherInfoPtr server_info,
    const std::string& publisher_key,
    ledger::GetServerPublisherInfoCallback callback) {
  if (ShouldFetchServerPublisherInfo(server_info.get())) {
    // Store the current server publisher info so that if fetching fails
    // we can execute the callback with the last known valid data.
    auto shared_info = std::make_shared<ledger::ServerPublisherInfoPtr>(
        std::move(server_info));

    FetchServerPublisherInfo(
        publisher_key,
        [shared_info, callback](ledger::ServerPublisherInfoPtr info) {
          callback(std::move(info ? info : *shared_info));
        });
    return;
  }

  callback(std::move(server_info));
}

void Publisher::UpdateMediaDuration(
    const std::string& publisher_key,
    uint64_t duration) {
  BLOG(1, "Media duration: " << duration);
  ledger_->database()->GetPublisherInfo(publisher_key,
      std::bind(&Publisher::OnGetPublisherInfoForUpdateMediaDuration,
                this,
                _1,
                _2,
                publisher_key,
                duration));
}

void Publisher::OnGetPublisherInfoForUpdateMediaDuration(
    ledger::Result result,
    ledger::PublisherInfoPtr info,
    const std::string& publisher_key,
    uint64_t duration) {
  if (result != ledger::Result::LEDGER_OK) {
    BLOG(0, "Failed to retrieve publisher info while updating media duration");
    return;
  }

  ledger_->database()->UpdateActivityInfoDuration(
      publisher_key,
      duration,
      [publisher_key](const ledger::Result result) {
        if (result != ledger::Result::LEDGER_OK) {
          BLOG(0, "Failed to update media duration for publisher");
          return;
        }
      });
}

void Publisher::GetPublisherPanelInfo(
    const std::string& publisher_key,
    ledger::GetPublisherInfoCallback callback) {
  auto filter = CreateActivityFilter(
      publisher_key,
      ledger::ExcludeFilter::FILTER_ALL,
      false,
      ledger_->state()->GetReconcileStamp(),
      true,
      false);

  ledger_->database()->GetPanelPublisherInfo(std::move(filter),
      std::bind(&Publisher::OnGetPanelPublisherInfo,
                this,
                _1,
                _2,
                callback));
}

void Publisher::OnGetPanelPublisherInfo(
    const ledger::Result result,
    ledger::PublisherInfoPtr info,
    ledger::GetPublisherInfoCallback callback) {
  if (result != ledger::Result::LEDGER_OK) {
    BLOG(0, "Failed to retrieve panel publisher info");
    callback(result, nullptr);
    return;
  }

  callback(result, std::move(info));
}

void Publisher::SavePublisherInfo(
    const uint64_t window_id,
    ledger::PublisherInfoPtr publisher_info,
    ledger::ResultCallback callback) {
  if (!publisher_info || publisher_info->id.empty()) {
    BLOG(0, "Publisher key is missing for url");
    callback(ledger::Result::LEDGER_ERROR);
    return;
  }

  ledger::VisitData visit_data;
  visit_data.provider = publisher_info->provider;
  visit_data.name = publisher_info->name;
  visit_data.url = publisher_info->url;
  if (!publisher_info->favicon_url.empty()) {
    visit_data.favicon_url = publisher_info->favicon_url;
  }

  SaveVideoVisit(
      publisher_info->id,
      visit_data,
      0,
      window_id,
      [=](auto result, ledger::PublisherInfoPtr publisher_info) {
        callback(result);
      });
}

}  // namespace braveledger_publisher
