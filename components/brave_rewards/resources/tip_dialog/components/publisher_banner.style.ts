/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import styled from 'styled-components'

import background1 from '../assets/background_1.svg'
import background2 from '../assets/background_2.svg'
import background3 from '../assets/background_3.svg'
import background4 from '../assets/background_4.svg'

export const loading = styled.div``

export const root = styled.div`
  display: flex;
  justify-content: center;
  padding: 64px 12px;
  height: 100%;

  background-color: #212529;
  background-size: cover;
  background-position: center;
  color: #fff;

  &.background-1 {
    background-image: url('${background1}');
  }

  &.background-2 {
    background-image: url('${background2}');
  }

  &.background-3 {
    background-image: url('${background3}');
  }

  &.background-4 {
    background-image: url('${background4}');
  }
`

export const card = styled.div`
  flex: 0 1 700px;
  padding: 32px 27px;

  &.background-custom {
    background: linear-gradient(
      102.21deg,
      rgba(255, 255, 255, 0.24) -8.14%,
      rgba(0, 0, 0, 0) 102.57%),
      rgba(81, 81, 81, 0.35
    );
    box-shadow: 0px 4.7475px 28.485px rgba(0, 0, 0, 0.16);
    backdrop-filter: blur(9.5px);
    border-radius: 9.5px;
  }
`

export const header = styled.div`
  display: grid;
  grid-template-columns: 106px auto auto;
  grid-template-areas:
    "logo name     name"
    "logo verified social";
`

export const logo = styled.div`
  grid-area: logo;
  height: 82px;
  width: 82px;
  text-align: center;
  overflow: hidden;

  background: #fff;
  border-radius: 50%;
  text-align: center;
  color: #FB542B;
  text-transform: uppercase;
  font-size: 52px;
  font-weight: 600;
  line-height: 82px;

  > img {
    object-fit: cover;
    max-height: 100%;
  }
`

export const name = styled.div`
  grid-area: name;
  align-self: end;

  font-size: 22px;
  line-height: 33px;
  font-weight: 600;
`

export const verifiedStatus = styled.div`
  grid-area: verified;
  padding-top: 10px;

  font-size: 14px;
  line-height: 21px;
  font-weight: normal;

  .icon {
    width: 18px;
    vertical-align: middle;
    margin: 0 3px 3px 0;
  }
`

export const unverifiedNotice = styled.div`
  padding: 7px 14px;
  margin-top: 15px;
  margin-bottom: -24px;
  display: flex;
  align-items: center;

  font-size: 12px;
  background: #fff;
  color: #000;
  border-radius: 4px;
`

export const unverifiedNoticeIcon = styled.div`
  .icon {
    width: 25px;
    margin-right: 10px;
  }
`

export const unverifiedNoticeText = styled.div`
  flex: 1 1 auto;

  a {
    font-weight: 600;
    color: #72b4f6;
  }
`

export const socialLinks = styled.div`
  grid-area: social;
  display: flex;
  justify-content: flex-end;
  align-items: flex-end;

  a {
    display: block;
    width: 25px;
    height: 25px;
    padding: 4px;
    margin: 0 5px;

    border-radius: 50%;
    background: #fff;
  }
`

export const title = styled.div`
  margin-top: 40px;

  font-size: 18px;
  line-height: 26px;
`

export const description = styled.div`
  margin-top: 5px;
`

export const media = styled.div`
  display: grid;
  grid-template-columns: 22px auto;
  grid-template-areas:
    "logo date"
    "text text";
  margin: 15px 0 0 0;
  padding: 15px;

  border-radius: 5px;
  background: #fff;
  color: #000;
`

export const mediaIcon = styled.div`
  grid-area: logo;
  margin-right: 5px;

  .icon {
    vertical-align: middle;
    margin-bottom: 3px;
  }
`

export const mediaDate = styled.div`
  grid-area: date;

  font-size: 12px;
  color: #AEB1C2;
`

export const mediaText = styled.div`
  grid-area: text;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: pre-wrap;
`
