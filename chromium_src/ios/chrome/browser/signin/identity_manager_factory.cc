// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ios/chrome/browser/signin/identity_manager_factory.h"

#include <memory>
#include <utility>

#include "components/image_fetcher/ios/ios_image_decoder_impl.h"
#include "components/keyed_service/core/keyed_service.h"
#include "components/keyed_service/ios/browser_state_dependency_manager.h"
#include "components/pref_registry/pref_registry_syncable.h"
#include "components/signin/public/base/account_consistency_method.h"
#include "components/signin/public/base/signin_client.h"
#include "components/signin/public/identity_manager/identity_manager.h"
#include "components/signin/public/identity_manager/identity_manager_builder.h"
#include "ios/chrome/browser/application_context.h"
#include "ios/chrome/browser/browser_state/chrome_browser_state.h"
#include "ios/chrome/browser/signin/device_accounts_provider_impl.h"
#include "ios/chrome/browser/signin/identity_manager_factory_observer.h"
#include "ios/chrome/browser/signin/signin_client_factory.h"

// TODO(bridiver) - fix this
#include "components/signin/public/identity_manager/ios/fake_device_accounts_provider.h"
#include "components/signin/public/base/signin_client.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"

namespace {

class FakeSigninClient : public SigninClient {
 public:
  FakeSigninClient(ChromeBrowserState* browser_state)
      : browser_state_(browser_state) {}
  ~FakeSigninClient() override {}
  void Shutdown() override {}
  PrefService* GetPrefs() override {
    return browser_state_->GetPrefs();
  }
  scoped_refptr<network::SharedURLLoaderFactory>
  GetURLLoaderFactory() override {
    return browser_state_->GetSharedURLLoaderFactory();
  }
  network::mojom::CookieManager* GetCookieManager() override {
    return browser_state_->GetCookieManager();
  }
  void DoFinalInit() override {}
  bool AreSigninCookiesAllowed() override { return true; }
  bool AreSigninCookiesDeletedOnExit() override { return true; }
  void AddContentSettingsObserver(
      content_settings::Observer* observer) override {}
  void RemoveContentSettingsObserver(
      content_settings::Observer* observer) override {}
  void DelayNetworkCall(base::OnceClosure callback) override {}
  std::unique_ptr<GaiaAuthFetcher> CreateGaiaAuthFetcher(
      GaiaAuthConsumer* consumer,
      gaia::GaiaSource source) override {
    return std::unique_ptr<GaiaAuthFetcher>();
  }
  void PreGaiaLogout(base::OnceClosure callback) override {}

 private:
  ChromeBrowserState* browser_state_;
  DISALLOW_COPY_AND_ASSIGN(FakeSigninClient);
};


}  // namespace

void IdentityManagerFactory::RegisterBrowserStatePrefs(
    user_prefs::PrefRegistrySyncable* registry) {
  signin::IdentityManager::RegisterProfilePrefs(registry);
}

IdentityManagerFactory::IdentityManagerFactory()
    : BrowserStateKeyedServiceFactory(
          "IdentityManager",
          BrowserStateDependencyManager::GetInstance()) {
  // DependsOn(SigninClientFactory::GetInstance());
}

IdentityManagerFactory::~IdentityManagerFactory() {}

// static
signin::IdentityManager* IdentityManagerFactory::GetForBrowserState(
    ChromeBrowserState* browser_state) {
  return static_cast<signin::IdentityManager*>(
      GetInstance()->GetServiceForBrowserState(browser_state, true));
}

// static
signin::IdentityManager* IdentityManagerFactory::GetForBrowserStateIfExists(
    ChromeBrowserState* browser_state) {
  return static_cast<signin::IdentityManager*>(
      GetInstance()->GetServiceForBrowserState(browser_state, false));
}

// static
IdentityManagerFactory* IdentityManagerFactory::GetInstance() {
  static base::NoDestructor<IdentityManagerFactory> instance;
  return instance.get();
}

void IdentityManagerFactory::AddObserver(
    IdentityManagerFactoryObserver* observer) {
  observer_list_.AddObserver(observer);
}

void IdentityManagerFactory::RemoveObserver(
    IdentityManagerFactoryObserver* observer) {
  observer_list_.RemoveObserver(observer);
}

std::unique_ptr<KeyedService> IdentityManagerFactory::BuildServiceInstanceFor(
    web::BrowserState* context) const {
  ChromeBrowserState* browser_state =
      ChromeBrowserState::FromBrowserState(context);

  signin::IdentityManagerBuildParams params;
  params.account_consistency = signin::AccountConsistencyMethod::kMirror;
  params.device_accounts_provider =
      std::make_unique<FakeDeviceAccountsProvider>();
      // std::make_unique<DeviceAccountsProviderImpl>();
  params.image_decoder = image_fetcher::CreateIOSImageDecoder();
  params.local_state = browser_state->GetPrefs();
      //GetApplicationContext()->GetLocalState();
  params.pref_service = browser_state->GetPrefs();
  params.profile_path = base::FilePath();
  // TODO(bridiver) - fix this, but don't worry about the leak for now
  params.signin_client = new FakeSigninClient(browser_state);

  std::unique_ptr<signin::IdentityManager> identity_manager =
      signin::BuildIdentityManager(&params);

  for (auto& observer : observer_list_)
    observer.IdentityManagerCreated(identity_manager.get());

  return identity_manager;
}

void IdentityManagerFactory::BrowserStateShutdown(web::BrowserState* context) {
  auto* identity_manager = static_cast<signin::IdentityManager*>(
      GetServiceForBrowserState(context, false));
  if (identity_manager) {
    for (auto& observer : observer_list_)
      observer.IdentityManagerShutdown(identity_manager);
  }
  BrowserStateKeyedServiceFactory::BrowserStateShutdown(context);
}
