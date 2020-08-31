/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

import {
  Host,
  BalanceInfo,
  PaymentKind,
  RewardsParameters,
  PublisherInfo
} from '../lib/interfaces'

import { ButtonSwitch, ButtonSwitchOption } from '../../shared/components/button_switch'
import { HostContext } from '../lib/host_context'
import { formatFiatAmount, formatLocaleTemplate } from '../lib/formatting'
import { PaymentKindSwitch } from './payment_kind_switch'

import { ExpandIcon } from './icons/expand_icon'
import { SadFaceIcon } from './icons/sad_face_icon'
import { PaperAirplaneIcon } from './icons/paper_airplane_icon'

import * as style from './one_time_tip_form.style'

function generateTipOptions (
  rewardsParameters: RewardsParameters,
  publisherInfo: PublisherInfo
) {
  const publisherAmounts = publisherInfo.amounts
  if (publisherAmounts && publisherAmounts.length > 0) {
    return publisherAmounts
  }
  const { tipChoices } = rewardsParameters
  if (tipChoices.length > 0) {
    return tipChoices
  }
  return [1, 5, 10]
}

function getTipAmountOptions (
  host: Host,
  tipOptions: number[],
  fiatRate: number
): ButtonSwitchOption<number>[] {
  // TODO: All of this needs to be looked at in terms of bap/bat
  return tipOptions.map((value) => ({
    value,
    content: <><strong>{value.toFixed(0)}</strong> {host.getString('bat')}</>,
    caption: formatFiatAmount(value, fiatRate)
  }))
}

function getInsufficientFundsContent (host: Host, onlyAnon: boolean) {
  if (onlyAnon) {
    return formatLocaleTemplate(
      host.getString('notEnoughTokens'),
      { currency: host.getString('points') }
    )
  }
  const text = formatLocaleTemplate(
    host.getString('notEnoughTokensLink'),
    { currency: host.getString('tokens') }
  )
  return <>{text} <a href='about:blank'>{host.getString('addFunds')}</a>.</>
}

export function OneTimeTipForm () {
  const host = React.useContext(HostContext)
  const { getString } = host

  const [balanceInfo, setBalanceInfo] = React.useState<BalanceInfo | undefined>()
  const [paymentKind, setPaymentKind] = React.useState<PaymentKind>('bat')
  const [rewardsParameters, setRewardsParameters] = React.useState<RewardsParameters | undefined>()
  const [publisherInfo, setPublisherInfo] = React.useState<PublisherInfo | undefined>()
  const [onlyAnon, setOnlyAnon] = React.useState<boolean>(false)
  const [tipAmount, setTipAmount] = React.useState<number>(0)

  React.useEffect(() => {
    host.addListener((state) => {
      setBalanceInfo(state.balanceInfo)
      setRewardsParameters(state.rewardsParameters)
      setPublisherInfo(state.publisherInfo)
      setOnlyAnon(Boolean(state.onlyAnonWallet))
    })
  }, [host])

  if (!balanceInfo || !rewardsParameters || !publisherInfo) {
    // TODO: Loading?
    return null
  }

  function processTip () {
    if (tipAmount > 0) {
      host.processTip(tipAmount, 'one-time')
    }
  }

  const tipOptions = generateTipOptions(rewardsParameters, publisherInfo)

  // TODO: Select a default tip amount automatically?

  return (
    <style.root>
      <style.main>
        <PaymentKindSwitch
          userBalance={balanceInfo.total}
          currentValue={paymentKind}
          onChange={setPaymentKind}
        />
        <style.amounts>
          <ButtonSwitch<number>
            options={getTipAmountOptions(host, tipOptions, rewardsParameters.rate)}
            selectedValue={tipAmount}
            onSelect={setTipAmount}
          />
        </style.amounts>
        <style.otherAmounts>
          {getString('otherAmounts')} <ExpandIcon />
        </style.otherAmounts>
      </style.main>
      <style.footer>
        <style.terms>
          <span dangerouslySetInnerHTML={{ __html: getString('sendTipTerms') }} />
        </style.terms>
        {
          balanceInfo.total < tipAmount ?
            <style.addFunds>
              <SadFaceIcon /> {getInsufficientFundsContent(host, onlyAnon)}
            </style.addFunds> :
            <style.submit>
              <button onClick={processTip}>
                <PaperAirplaneIcon /> {getString('sendDonation')}
              </button>
            </style.submit>
        }
      </style.footer>
    </style.root>
  )
}
