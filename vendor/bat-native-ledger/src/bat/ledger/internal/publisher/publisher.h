/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_PUBLISHER_PUBLISHER_H_
#define BRAVELEDGER_PUBLISHER_PUBLISHER_H_

#include <string>
#include <map>
#include <memory>
#include <vector>

#include "base/gtest_prod_util.h"
#include "bat/ledger/ledger.h"

namespace bat_ledger {
class LedgerImpl;
}

namespace braveledger_publisher {

class PublisherPrefixListUpdater;
class ServerPublisherFetcher;

class Publisher {
 public:
  explicit Publisher(bat_ledger::LedgerImpl* ledger);

  ~Publisher();

  bool ShouldFetchServerPublisherInfo(
      ledger::ServerPublisherInfo* server_info);

  void FetchServerPublisherInfo(
      const std::string& publisher_key,
      ledger::GetServerPublisherInfoCallback callback);

  void RefreshPublisher(
      const std::string& publisher_key,
      ledger::OnRefreshPublisherCallback callback);

  void SetPublisherServerListTimer();

  void SaveVisit(const std::string& publisher_key,
                 const ledger::VisitData& visit_data,
                 const uint64_t& duration,
                 uint64_t window_id,
                 const ledger::PublisherInfoCallback callback);

  void SaveVideoVisit(
      const std::string& publisher_id,
      const ledger::VisitData& visit_data,
      uint64_t duration,
      uint64_t window_id,
      ledger::PublisherInfoCallback callback);

  void SetPublisherExclude(
      const std::string& publisher_id,
      const ledger::PublisherExclude& exclude,
      ledger::ResultCallback callback);

  void OnPublisherInfoSaved(const ledger::Result result);

  void GetPublisherActivityFromUrl(
      uint64_t windowId,
      ledger::VisitDataPtr visit_data,
      const std::string& publisher_blob);

  void GetPublisherBanner(const std::string& publisher_key,
                          ledger::PublisherBannerCallback callback);

  ledger::ActivityInfoFilterPtr CreateActivityFilter(
      const std::string& publisher_id,
      ledger::ExcludeFilter excluded,
      bool min_duration,
      const uint64_t& current_reconcile_stamp,
      bool non_verified,
      bool min_visits);

  void NormalizeContributeWinners(
      ledger::PublisherInfoList* newList,
      const ledger::PublisherInfoList* list,
      uint32_t /* next_record */);

  void OnRestorePublishers(
      const ledger::Result result,
      ledger::ResultCallback callback);

  bool IsConnectedOrVerified(const ledger::PublisherStatus status);

  void SynopsisNormalizer();

  void CalcScoreConsts(const int min_duration_seconds);

  void GetServerPublisherInfo(
      const std::string& publisher_key,
      ledger::GetServerPublisherInfoCallback callback);

  void UpdateMediaDuration(const std::string& publisher_key, uint64_t duration);

  void GetPublisherPanelInfo(
      const std::string& publisher_key,
      ledger::GetPublisherInfoCallback callback);

  void SavePublisherInfo(
      const uint64_t window_id,
      ledger::PublisherInfoPtr publisher_info,
      ledger::ResultCallback callback);

 private:
  void OnGetPublisherInfoForUpdateMediaDuration(
      ledger::Result result,
      ledger::PublisherInfoPtr info,
      const std::string& publisher_key,
      uint64_t duration);

  void OnGetPanelPublisherInfo(
      const ledger::Result result,
      ledger::PublisherInfoPtr info,
      ledger::GetPublisherInfoCallback callback);

  void onPublisherActivitySave(
      uint64_t windowId,
      const ledger::VisitData& visit_data,
      ledger::Result result,
      ledger::PublisherInfoPtr info);

  void OnGetActivityInfo(
      ledger::PublisherInfoList list,
      ledger::PublisherInfoCallback callback,
      const std::string& publisher_key);

  void SaveVisitInternal(
      const ledger::PublisherStatus,
      const std::string& publisher_key,
      const ledger::VisitData& visit_data,
      uint64_t duration,
      uint64_t window_id,
      const ledger::PublisherInfoCallback callback,
      ledger::Result result,
      ledger::PublisherInfoPtr publisher_info);

  void OnSaveVisitServerPublisher(
    ledger::ServerPublisherInfoPtr server_info,
    const std::string& publisher_key,
    const ledger::VisitData& visit_data,
    uint64_t duration,
    uint64_t window_id,
    const ledger::PublisherInfoCallback callback);

  void onFetchFavIcon(const std::string& publisher_key,
                      uint64_t window_id,
                      bool success,
                      const std::string& favicon_url);

  void onFetchFavIconDBResponse(ledger::Result result,
                                ledger::PublisherInfoPtr info,
                                const std::string& favicon_url,
                                uint64_t window_id);

  void OnSetPublisherExclude(
    ledger::PublisherExclude exclude,
    ledger::Result result,
    ledger::PublisherInfoPtr publisher_info,
    ledger::ResultCallback callback);

  double concaveScore(const uint64_t& duration_seconds);

  void SynopsisNormalizerCallback(ledger::PublisherInfoList list);

  void synopsisNormalizerInternal(ledger::PublisherInfoList* newList,
                                  const ledger::PublisherInfoList* list,
                                  uint32_t /* next_record */);

  void OnSaveVisitInternal(
    ledger::Result result,
    ledger::PublisherInfoPtr info);

  void OnPanelPublisherInfo(
      ledger::Result result,
      ledger::PublisherInfoPtr publisher_info,
      uint64_t windowId,
      const ledger::VisitData& visit_data);

  void OnGetPublisherBanner(
      ledger::ServerPublisherInfoPtr info,
      const std::string& publisher_key,
      ledger::PublisherBannerCallback callback);

  void OnGetPublisherBannerPublisher(
      ledger::PublisherBannerCallback callback,
      const ledger::PublisherBanner& banner,
      ledger::Result result,
      ledger::PublisherInfoPtr publisher_info);

  ledger::PublisherStatus ParsePublisherStatus(const std::string& status);

  void OnServerPublisherInfoLoaded(
      ledger::ServerPublisherInfoPtr server_info,
      const std::string& publisher_key,
      ledger::GetServerPublisherInfoCallback callback);

  bat_ledger::LedgerImpl* ledger_;  // NOT OWNED
  std::unique_ptr<PublisherPrefixListUpdater> prefix_list_updater_;
  std::unique_ptr<ServerPublisherFetcher> server_publisher_fetcher_;

  // For testing purposes
  friend class PublisherTest;
  FRIEND_TEST_ALL_PREFIXES(PublisherTest, concaveScore);
  FRIEND_TEST_ALL_PREFIXES(PublisherTest, synopsisNormalizerInternal);
};

}  // namespace braveledger_publisher

#endif  // BRAVELEDGER_PUBLISHER_PUBLISHER_H_
