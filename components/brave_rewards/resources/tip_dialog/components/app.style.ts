/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import styled from 'styled-components'

export const root = styled.div`
  display: flex;
  min-height: 425px;

  font-family: Poppins, sans-serif;
  font-size: 14px;
  line-height: 22px;

  a {
    text-transform: none;
  }

  strong {
    font-weight: 600;
  }
`

export const banner = styled.div`
  flex: 1 1 auto;
`

export const form = styled.div`
  position: relative;
  flex: 0 0 364px;
  background: #fff;
`

export const close = styled.div`
  button {
    position: absolute;
    top: 11px;
    right: 10px;
    padding: 10px;
    background: none;
    border: none;
    border-radius: 6px;
    cursor: pointer;
    color: #D8D8D8;
  }

  .icon {
    display: block;
    height: 12px;
  }
`
