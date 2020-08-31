/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

import {
  Host,
  BalanceInfo,
  PublisherInfo,
  RewardsParameters,
  TipKind
} from '../lib/interfaces'

import { HostContext } from '../lib/host_context'

import { SliderSwitch, SliderSwitchOption } from '../../shared/components/slider_switch'

import { TipComplete } from './tip_complete'
import { OneTimeTipForm } from './one_time_tip_form'
import { MonthlyTipForm } from './monthly_tip_form'

import * as style from './tip_form.style'

function getTipKindOptions (host: Host): SliderSwitchOption<TipKind>[] {
  return [
    { value: 'one-time', content: host.getString('oneTimeTip') },
    { value: 'monthly', content: host.getString('monthlyText') }
  ]
}

export function TipForm () {
  const host = React.useContext(HostContext)
  const { getString } = host

  const [balanceInfo, setBalanceInfo] = React.useState<BalanceInfo | undefined>()
  const [publisherInfo, setPublisherInfo] = React.useState<PublisherInfo | undefined>()
  const [rewardsParameters, setRewardsParameters] = React.useState<RewardsParameters | undefined>()
  const [tipAmount, setTipAmount] = React.useState<number>(0)
  const [tipProcessed, setTipProcessed] = React.useState<boolean>(false)

  const [tipKind, setTipKind] = React.useState<TipKind>(() => {
    return host.getDialogArgs().monthly ? 'monthly' : 'one-time'
  })

  React.useEffect(() => {
    setTipAmount(0)
    return host.addListener((state) => {
      setTipProcessed(Boolean(state.tipProcessed))
      setTipAmount(state.tipAmount || 0)
      setRewardsParameters(state.rewardsParameters)
      setPublisherInfo(state.publisherInfo)
      setBalanceInfo(state.balanceInfo)
    })
  }, [host])

  if (!rewardsParameters || !publisherInfo || !balanceInfo) {
    return <style.loading />
  }

  if (tipProcessed) {
    return <TipComplete tipKind={tipKind} tipAmount={tipAmount} />
  }

  return (
    <style.root>
      <style.header>
        {getString('supportThisCreator')}
      </style.header>
      <style.tipKind>
        <SliderSwitch<TipKind>
          options={getTipKindOptions(host)}
          selectedValue={tipKind}
          onSelect={setTipKind}
        />
      </style.tipKind>
      <style.main>
        {tipKind === 'one-time' ? <OneTimeTipForm /> : <MonthlyTipForm />}
      </style.main>
    </style.root>
  )
}
