/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

type EmptyMediaData = {
  mediaType: 'none'
}

export type TwitterMediaData = {
  mediaType: 'twitter'
  twitterName: string
  screenName: string
  userId: string
  tweetId: string
  tweetTimestamp: number
  tweetText: string
}

type RedditMediaData = {
  mediaType: 'reddit'
  userName: string
  postText: string
  postRelDate: string
}

type GithubMediaData = {
  mediaType: 'github'
  userName: string
}

export type ShareTarget = 'twitter'

export interface DialogArgs {
  url: string
  monthly: boolean
  publisherKey: string
  mediaMetaData:
    EmptyMediaData |
    TwitterMediaData |
    RedditMediaData |
    GithubMediaData
}

export type TipKind = 'one-time' | 'monthly'

export type PaymentKind = 'bat'

export enum PublisherStatus {
  NOT_VERIFIED = 0,
  CONNECTED = 1,
  VERIFIED = 2
}

export interface PublisherInfo {
  publisherKey: string
  name: string
  title: string
  description: string
  background: string
  logo: string
  amounts: number[]
  provider: string
  links: Record<string, string>
  status: PublisherStatus
}

export interface BalanceInfo {
  total: number
  wallets: Record<string, number>
}

export enum ExternalWalletStatus {
  NOT_CONNECTED = 0,
  CONNECTED = 1,
  VERIFIED = 2,
  DISCONNECTED_NOT_VERIFIED = 3,
  DISCONNECTED_VERIFIED = 4
}

export interface ExternalWalletInfo {
  token: string
  address: string
  status: ExternalWalletStatus
  type: 'anonymous' | 'uphold'
  verifyUrl: string
  addUrl: string
  withdrawUrl: string
  userName: string
  accountUrl: string
  loginUrl: string
}

export interface RewardsParameters {
  rate: number
  tipChoices: number[]
  monthlyTipChoices: number[]
}

export type StringKey =
  'addFunds' |
  'bat' |
  'batFunds' |
  'braveUnverifiedCreator' |
  'braveVerifiedCreator' |
  'contributionAmountLabel' |
  'doMonthly' |
  'githubTipTitle' |
  'githubTipTitleEmpty' |
  'monthlyContributionSet' |
  'monthlyText' |
  'nextContributionDate' |
  'notEnoughTokens' |
  'notEnoughTokensLink' |
  'on' |
  'oneTimeTip' |
  'oneTimeTipAmount' |
  'otherAmounts' |
  'points' |
  'redditTipTitle' |
  'redditTipTitleEmpty' |
  'sendDonation' |
  'sendTipTerms' |
  'setContributionTerms' |
  'siteBannerConnectedText' |
  'siteBannerNoticeNote' |
  'siteBannerNoticeText' |
  'supportThisCreator' |
  'thanksForTheSupport' |
  'tipHasBeenSent' |
  'tokens' |
  'tweetAboutSupport' |
  'tweetTipTitle' |
  'tweetTipTitleEmpty' |
  'unVerifiedTextMore' |
  'welcome'

export interface HostError {
  type: string
  code?: number
}

export interface HostState {
  publisherInfo?: PublisherInfo
  balanceInfo?: BalanceInfo
  externalWalletInfo?: ExternalWalletInfo
  rewardsParameters?: RewardsParameters
  hostError?: HostError
  nextReconcileDate?: Date
  // TODO: Combine these?
  onlyAnonWallet?: boolean
  tipProcessed?: boolean
  tipAmount?: number
}

export type HostListener = (state: HostState) => void

export interface Host {
  getString: (key: StringKey) => string
  getDialogArgs: () => DialogArgs
  closeDialog: () => void
  processTip: (amount: number, kind: TipKind) => void
  shareTip: (target: ShareTarget) => void
  addListener: (callback: HostListener) => () => void
}
