/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#import "NativeLedgerClient.h"
#import "NativeLedgerClientBridge.h"

// Constructor & Destructor
NativeLedgerClient::NativeLedgerClient(id<NativeLedgerClientBridge> bridge) : bridge_(bridge) { }
NativeLedgerClient::~NativeLedgerClient() {
  bridge_ = nil;
}

void NativeLedgerClient::FetchFavIcon(const std::string & url, const std::string & favicon_key, ledger::FetchIconCallback callback) {
  [bridge_ fetchFavIcon:url faviconKey:favicon_key callback:callback];
}
void NativeLedgerClient::LoadLedgerState(ledger::OnLoadCallback callback) {
  [bridge_ loadLedgerState:callback];
}
void NativeLedgerClient::LoadPublisherState(ledger::OnLoadCallback callback) {
  [bridge_ loadPublisherState:callback];
}
void NativeLedgerClient::LoadURL(ledger::UrlRequestPtr request, ledger::LoadURLCallback callback) {
  [bridge_ loadURL:std::move(request) callback:callback];
}
void NativeLedgerClient::Log(const char * file, const int line, const int verbose_level, const std::string & message) {
  [bridge_ log:file line:line verboseLevel:verbose_level message:message];
}
void NativeLedgerClient::OnPanelPublisherInfo(ledger::Result result, ledger::PublisherInfoPtr publisher_info, uint64_t windowId) {
  [bridge_ onPanelPublisherInfo:result publisherInfo:std::move(publisher_info) windowId:windowId];
}
void NativeLedgerClient::OnReconcileComplete(ledger::Result result, ledger::ContributionInfoPtr contribution) {
  [bridge_ onReconcileComplete:result contribution:std::move(contribution)];
}
void NativeLedgerClient::PublisherListNormalized(ledger::PublisherInfoList list) {
  [bridge_ publisherListNormalized:std::move(list)];
}
std::string NativeLedgerClient::URIEncode(const std::string & value) {
  return [bridge_ URIEncode:value];
}
void NativeLedgerClient::OnContributeUnverifiedPublishers(ledger::Result result, const std::string& publisher_key, const std::string& publisher_name) {
  return [bridge_ onContributeUnverifiedPublishers:result publisherKey:publisher_key publisherName:publisher_name];
}
void NativeLedgerClient::SetBooleanState(const std::string& name, bool value) {
  [bridge_ setBooleanState:name value:value];
}
bool NativeLedgerClient::GetBooleanState(const std::string& name) const {
  return [bridge_ getBooleanState:name];
}
void NativeLedgerClient::SetIntegerState(const std::string& name, int value) {
  [bridge_ setIntegerState:name value:value];
}
int NativeLedgerClient::GetIntegerState(const std::string& name) const {
  return [bridge_ getIntegerState:name];
}
void NativeLedgerClient::SetDoubleState(const std::string& name, double value) {
  [bridge_ setDoubleState:name value:value];
}
double NativeLedgerClient::GetDoubleState(const std::string& name) const {
  return [bridge_ getDoubleState:name];
}
void NativeLedgerClient::SetStringState(const std::string& name, const std::string& value) {
  [bridge_ setStringState:name value:value];
}
std::string NativeLedgerClient::GetStringState(const std::string& name) const {
  return [bridge_ getStringState:name];
}
void NativeLedgerClient::SetInt64State(const std::string& name, int64_t value) {
  [bridge_ setInt64State:name value:value];
}
int64_t NativeLedgerClient::GetInt64State(const std::string& name) const {
  return [bridge_ getInt64State:name];
}
void NativeLedgerClient::SetUint64State(const std::string& name, uint64_t value) {
  [bridge_ setUint64State:name value:value];
}
uint64_t NativeLedgerClient::GetUint64State(const std::string& name) const {
  return [bridge_ getUint64State:name];
}
void NativeLedgerClient::ClearState(const std::string& name) {
  [bridge_ clearState:name];
}
std::map<std::string, ledger::ExternalWalletPtr> NativeLedgerClient::GetExternalWallets() {
  return [bridge_ getExternalWallets];
}
void NativeLedgerClient::SaveExternalWallet(const std::string& wallet_type, ledger::ExternalWalletPtr wallet) {
  [bridge_ saveExternalWallet:wallet_type wallet:std::move(wallet)];
}
void NativeLedgerClient::ShowNotification(const std::string& type, const std::vector<std::string>& args, ledger::ResultCallback callback) {
  [bridge_ showNotification:type args:args callback:callback];
}
void NativeLedgerClient::SetTransferFee(const std::string& wallet_type, ledger::TransferFeePtr transfer_fee) {
  [bridge_ setTransferFee:wallet_type transfer_fee:std::move(transfer_fee)];
}
ledger::TransferFeeList NativeLedgerClient::GetTransferFees(const std::string& wallet_type) {
  return [bridge_ getTransferFees:wallet_type];
}
void NativeLedgerClient::RemoveTransferFee(const std::string& wallet_type, const std::string& id) {
  [bridge_ removeTransferFee:wallet_type id:id];
}
bool NativeLedgerClient::GetBooleanOption(const std::string& name) const {
  return [bridge_ getBooleanOption:name];
}
int NativeLedgerClient::GetIntegerOption(const std::string& name) const {
  return [bridge_ getIntegerOption:name];
}
double NativeLedgerClient::GetDoubleOption(const std::string& name) const {
  return [bridge_ getDoubleOption:name];
}
std::string NativeLedgerClient::GetStringOption(const std::string& name) const {
  return [bridge_ getStringOption:name];
}
int64_t NativeLedgerClient::GetInt64Option(const std::string& name) const {
  return [bridge_ getInt64Option:name];
}
uint64_t NativeLedgerClient::GetUint64Option(const std::string& name) const {
  return [bridge_ getUint64Option:name];
}
ledger::ClientInfoPtr NativeLedgerClient::GetClientInfo() {
  return [bridge_ getClientInfo];
}
void NativeLedgerClient::UnblindedTokensReady() {
  [bridge_ unblindedTokensReady];
}
void NativeLedgerClient::ReconcileStampReset() {
  [bridge_ reconcileStampReset];
}
void NativeLedgerClient::RunDBTransaction(ledger::DBTransactionPtr transaction, ledger::RunDBTransactionCallback callback) {
  [bridge_ runDBTransaction:std::move(transaction) callback:callback];
}
void NativeLedgerClient::GetCreateScript(ledger::GetCreateScriptCallback callback) {
  [bridge_ getCreateScript:callback];
}
void NativeLedgerClient::PendingContributionSaved(const ledger::Result result) {
  [bridge_ pendingContributionSaved:result];
}
void NativeLedgerClient::ClearAllNotifications() {
  [bridge_ clearAllNotifications];
}
void NativeLedgerClient::WalletDisconnected(const std::string& wallet_type) {
  [bridge_ walletDisconnected:wallet_type];
}
void NativeLedgerClient::DeleteLog(ledger::ResultCallback callback) {
  [bridge_ deleteLog:callback];
}
