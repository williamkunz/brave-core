/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

import { HostContext } from '../lib/host_context'

import { PublisherBanner } from './publisher_banner'
import { TipForm } from './tip_form'

import { CloseIcon } from './icons/close_icon'

import * as style from './app.style'

export function App () {
  const host = React.useContext(HostContext)
  return (
    <style.root>
      <style.banner>
        <PublisherBanner />
      </style.banner>
      <style.form>
        <style.close>
          <button onClick={host.closeDialog}><CloseIcon /></button>
        </style.close>
        <TipForm />
      </style.form>
    </style.root>
  )
}
