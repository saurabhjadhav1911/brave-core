// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

export function getLoadTimeDataString (key: string) {
  if (
    (window as any) === undefined ||
    (window as any).loadTimeData === undefined ||
    (window as any).loadTimeData.getString === undefined
  ) {
    return key
  }
  return (window as any).loadTimeData.getString(key)
}

export function getLoadTimeDataBoolean (key: string, forceBooleanType?: boolean) {
  if (
    (window as any) === undefined ||
    (window as any).loadTimeData === undefined ||
    (window as any).loadTimeData.getBoolean === undefined
  ) {
    return !!forceBooleanType
  }
  return (window as any).loadTimeData.getBoolean(key)
}

export function getChromeDataBoolean (key: string, forceBooleanType?: boolean) {
  if (
    (chrome as any) === undefined ||
    (chrome as any).extension === undefined ||
    (chrome as any).extension[key] === undefined
  ) {
    return !!forceBooleanType
  }
  return chrome.extension[key]
}
