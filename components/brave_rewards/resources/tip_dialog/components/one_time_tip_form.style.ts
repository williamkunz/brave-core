/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import styled from 'styled-components'

export const root = styled.div`
  display: flex;
  flex-direction: column;
  height: 100%;
`

export const main = styled.div`
  flex: 1 1 auto;
  padding: 0 32px;
`

export const amounts = styled.div`
  padding-top: 24px;
  margin-top: 11px;

  border-top: 1px solid rgba(174, 177, 194, 0.5);
  font-size: 12px;

  strong {
    font-size: 14px;
  }
}
`

export const otherAmounts = styled.div`
  margin-top: 32px;
  text-align: center;

  font-size: 14px;
  line-height: 21px;
  font-weight: 600;
  color: #4C54D2;

  .icon {
    width: 12px;
    vertical-align: middle;
    margin: 0 0 2px 1px;
  }
`

export const footer = styled.div``

export const terms = styled.div`
  text-align: center;
  padding-bottom: 7px;

  font-size: 10px;
  line-height: 14px;
  color: #686978;

  a {
    font-weight: 600;
    color: inherit;
  }
`

export const addFunds = styled.div`
  padding: 19px 0;
  text-align: center;

  color: #fff;
  background: #1b1e2d;
  font-size: 12px;
  line-height: 21px;

  .icon {
    height: 18px;
    vertical-align: middle;
    margin: 0 4px 1.5px 0;
  }

  a {
    color: #72b4f6;;
  }
`

export const submit = styled.div`
  button {
    width: 100%;
    padding: 19px 0;

    cursor: pointer;
    border: none;
    background: #4C54D2;
    font-size: 14px;
    line-height: 21px;
    font-weight: 600;
    color: #fff;
  }

  button:active {
    background: #686fdc;
  }

  .icon {
    height: 16px;
    vertical-align: middle;
    margin-right: 5px;
  }
`
