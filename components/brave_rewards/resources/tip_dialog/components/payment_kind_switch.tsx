/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

import { BatLogoIcon } from './icons/bat_logo_icon'

import { ButtonSwitch, ButtonSwitchOption } from '../../shared/components/button_switch'
import { HostContext } from '../lib/host_context'
import { PaymentKind } from '../lib/interfaces'
import { formatTokenAmount } from '../lib/formatting'

import * as style from './payment_kind_switch.style'

interface Props {
  userBalance: number
  currentValue: PaymentKind
  onChange: (paymentKind: PaymentKind) => void
}

export function PaymentKindSwitch (props: Props) {
  const { getString } = React.useContext(HostContext)

  const options: ButtonSwitchOption<PaymentKind>[] = [
    {
      value: 'bat',
      content: (
        <style.batOption>
          <BatLogoIcon /> {getString('batFunds')}
        </style.batOption>
      ),
      caption: `${formatTokenAmount(props.userBalance)} ${getString('bat')}`
    }
  ]

  return (
    <style.root>
      <ButtonSwitch<PaymentKind>
        options={options}
        selectedValue={props.currentValue}
        onSelect={props.onChange}
      />
    </style.root>
  )
}
