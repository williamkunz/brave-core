/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

import { Section } from '../style'

interface Props {
  peerCount: number
}

export class ConnectedPeers extends React.Component<Props, {}> {
  constructor (props: Props) {
    super(props)
  }

  render () {
    return (
      <Section>
        <div>
          <span i18n-content='connectedPeersTitle'/>
          {this.props.peerCount}
        </div>
      </Section>
    )
  }
}
