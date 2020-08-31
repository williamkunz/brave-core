/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

import {
  Host,
  PublisherInfo,
  PublisherStatus,
  ExternalWalletInfo,
  ExternalWalletStatus,
  BalanceInfo
} from '../lib/interfaces'

import { HostContext } from '../lib/host_context'
import { formatLocaleTemplate } from '../lib/formatting'

import { ExclamationIcon } from './icons/exclamation_icon'
import { UnverifiedIcon } from './icons/unverified_icon'
import { VerifiedIcon } from './icons/verified_icon'
import { RedditIcon } from './icons/reddit_icon'
import { TwitchIcon } from './icons/twitch_icon'
import { TwitterIcon } from './icons/twitter_icon'
import { GitHubIcon } from './icons/github_icon'
import { YouTubeIcon } from './icons/youtube_icon'

import * as style from './publisher_banner.style'

function getLogoURL (publisherInfo: PublisherInfo) {
  const { logo } = publisherInfo
  if (!logo || publisherInfo.status === PublisherStatus.NOT_VERIFIED) {
    return ''
  }
  if (/^https:\/\/[a-z0-9-]+\.invalid(\/)?$/.test(logo)) {
    return `chrome://favicon/size/160@1x/${logo}`
  }
  return logo
}

function getLogo (publisherInfo: PublisherInfo) {
  const logoURL = getLogoURL(publisherInfo)
  if (logoURL) {
    return <img src={logoURL} />
  }
  if (publisherInfo.name) {
    return publisherInfo.name[0]
  }
  if (publisherInfo.publisherKey) {
    return publisherInfo.publisherKey[0]
  }
  return ''
}

function getProviderName (publisherInfo: PublisherInfo) {
  switch (publisherInfo.provider) {
    case 'youtube': return 'YouTube'
    case 'twitter': return 'Twitter'
    case 'twitch': return 'Twitch'
    case 'reddit': return 'Reddit'
    case 'vimeo': return 'Vimeo'
    case 'github': return 'GitHub'
    default: return ''
  }
}

function getPublisherName (host: Host, publisherInfo: PublisherInfo) {
  const name = publisherInfo.name || publisherInfo.publisherKey
  const platform = getProviderName(publisherInfo)
  return platform ? `${name} ${host.getString('on')} ${platform}` : name
}

function getSocialIcon (type: string) {
  switch (type) {
    case 'twitter': return <TwitterIcon />
    case 'youtube': return <YouTubeIcon />
    case 'twitch': return <TwitchIcon />
    case 'github': return <GitHubIcon />
    case 'reddit': return <RedditIcon />
    default: return null
  }
}

function getSocialLinks (publisherInfo: PublisherInfo) {
  return Object.entries(publisherInfo.links).map(([type, url]) => {
    const icon = getSocialIcon(type)
    return icon
      ? <a key={type} href={url} target='_blank'>{icon}</a>
      : null
  })
}

function getTitle (host: Host, publisherInfo: PublisherInfo) {
  const { mediaMetaData } = host.getDialogArgs()

  if (mediaMetaData.mediaType === 'twitter') {
    return formatLocaleTemplate(
      host.getString(mediaMetaData.tweetText
        ? 'tweetTipTitle'
        : 'tweetTipTitleEmpty'),
      { user: mediaMetaData.screenName })
  }

  if (mediaMetaData.mediaType === 'reddit') {
    return formatLocaleTemplate(
      host.getString(mediaMetaData.postText
        ? 'redditTipTitle'
        : 'redditTipTitleEmpty'),
      { user: 'u/' + mediaMetaData.userName })
  }

  if (mediaMetaData.mediaType === 'github') {
    return formatLocaleTemplate(
      host.getString(mediaMetaData.userName
        ? 'githubTipTitle'
        : 'githubTipTitleEmpty'),
      { user: '@' + mediaMetaData.userName })
  }

  return publisherInfo.title || host.getString('welcome')
}

function getVerifiedMessage (host: Host, publisherInfo: PublisherInfo) {
  if (publisherInfo.status === PublisherStatus.NOT_VERIFIED) {
    return (
      <span>
        <UnverifiedIcon />
        {host.getString('braveUnverifiedCreator')}
      </span>
    )
  }
  return (
    <span>
      <VerifiedIcon />
      {host.getString('braveVerifiedCreator')}
    </span>
  )
}

function showUnverifiedNotice (
  publisherInfo: PublisherInfo,
  balanceInfo?: BalanceInfo,
  externalWalletInfo?: ExternalWalletInfo
) {
  switch (publisherInfo.status) {
    case PublisherStatus.NOT_VERIFIED: return true
    case PublisherStatus.VERIFIED: return false
  }

  const hasNonUserFunds = Boolean(balanceInfo && (
    balanceInfo.wallets['anonymous'] ||
    balanceInfo.wallets['blinded']
  ))

  const walletConnectedOrVerified = Boolean(externalWalletInfo && (
    externalWalletInfo.status === ExternalWalletStatus.CONNECTED ||
    externalWalletInfo.status === ExternalWalletStatus.VERIFIED
  ))

  return hasNonUserFunds && walletConnectedOrVerified
}

function getUnverifiedNotice (
  host: Host,
  publisherInfo: PublisherInfo,
  balanceInfo?: BalanceInfo,
  walletInfo?: ExternalWalletInfo
) {
  if (!showUnverifiedNotice(publisherInfo, balanceInfo, walletInfo)) {
    return null
  }

  const { getString } = host

  const text = getString(publisherInfo.status === PublisherStatus.CONNECTED
    ? 'siteBannerConnectedText'
    : 'siteBannerNoticeText')

  return (
    <style.unverifiedNotice>
      <style.unverifiedNoticeIcon>
        <ExclamationIcon />
      </style.unverifiedNoticeIcon>
      <style.unverifiedNoticeText>
        <strong>{getString('siteBannerNoticeNote')}</strong>&nbsp;
        {text}&nbsp;
        <a href='https://brave.com/faq/#unclaimed-funds' target='_blank'>
          {getString('unVerifiedTextMore')}
        </a>
      </style.unverifiedNoticeText>
    </style.unverifiedNotice>
  )
}

function getDescription (host: Host, publisherInfo: PublisherInfo) {
  const { mediaMetaData } = host.getDialogArgs()

  if (mediaMetaData.mediaType === 'twitter') {
    const mediaDate = new Date(mediaMetaData.tweetTimestamp * 1000)
    const dateString = mediaDate.toLocaleDateString(undefined, {
      month: 'short',
      day: 'numeric',
      year: mediaDate.getFullYear() !== new Date().getFullYear()
        ? 'numeric'
        : undefined
    })
    return (
      <style.media>
        <style.mediaIcon><TwitterIcon /></style.mediaIcon>
        <style.mediaDate>{dateString}</style.mediaDate>
        <style.mediaText>{mediaMetaData.tweetText}</style.mediaText>
      </style.media>
    )
  }

  if (mediaMetaData.mediaType === 'reddit') {
    return (
      <style.media>
        <style.mediaIcon><RedditIcon /></style.mediaIcon>
        <style.mediaDate>{mediaMetaData.postRelDate}</style.mediaDate>
        <style.mediaText>{mediaMetaData.postText}</style.mediaText>
      </style.media>
    )
  }

  return publisherInfo.description
}

function hashString (input: string) {
  let h = 0
  for (let i = 0; i < input.length; i++) {
    h = (((h << 5) - h) + input.charCodeAt(i)) >>> 0
  }
  return h
}

export function PublisherBanner () {
  const host = React.useContext(HostContext)

  const [publisherInfo, setPublisherInfo] = React.useState<PublisherInfo | undefined>()
  const [balanceInfo, setBalanceInfo] = React.useState<BalanceInfo | undefined>()
  const [walletInfo, setWalletInfo] = React.useState<ExternalWalletInfo | undefined>()

  React.useEffect(() => {
    host.addListener((state) => {
      setPublisherInfo(state.publisherInfo)
      setBalanceInfo(state.balanceInfo)
      setWalletInfo(state.externalWalletInfo)
    })
  }, [host])

  if (!publisherInfo) {
    return <style.loading />
  }

  const backgroundClass = publisherInfo.background
    ? 'background-custom'
    : `background-${hashString(publisherInfo.publisherKey) % 4 + 1}`

  const backgroundImage = publisherInfo.background
    ? `url(${publisherInfo.background})`
    : ''

  return (
    <style.root className={backgroundClass} style={{ backgroundImage }}>
      <style.card className={backgroundClass}>
        <style.header>
          <style.logo>
            {getLogo(publisherInfo)}
          </style.logo>
          <style.name>
            {getPublisherName(host, publisherInfo)}
          </style.name>
          <style.verifiedStatus>
            {getVerifiedMessage(host, publisherInfo)}
          </style.verifiedStatus>
          <style.socialLinks>
            {getSocialLinks(publisherInfo)}
          </style.socialLinks>
        </style.header>
        {getUnverifiedNotice(host, publisherInfo, balanceInfo, walletInfo)}
        <style.title>
          {getTitle(host, publisherInfo)}
        </style.title>
        <style.description>
          {getDescription(host, publisherInfo)}
        </style.description>
      </style.card>
    </style.root>
  )
}
