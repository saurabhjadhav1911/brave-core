/* Copyright 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_perf_predictor/browser/p3a_bandwidth_savings_permanent_state.h"

#include <numeric>
#include <utility>

#include "base/logging.h"
#include "base/metrics/histogram_macros.h"
#include "brave/components/brave_perf_predictor/common/pref_names.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"
#include "components/prefs/scoped_user_pref_update.h"

namespace brave_perf_predictor {

P3ABandwidthSavingsPermanentState::P3ABandwidthSavingsPermanentState(
    PrefService* user_prefs)
    : user_prefs_(user_prefs) {
  if (user_prefs)
    LoadSavingsDaily();
}

P3ABandwidthSavingsPermanentState::~P3ABandwidthSavingsPermanentState() =
    default;

void P3ABandwidthSavingsPermanentState::AddSavings(uint64_t delta) {
  base::Time now_midnight = base::Time::Now().LocalMidnight();
  base::Time last_saved_midnight;

  if (!daily_savings_.empty())
    last_saved_midnight = daily_savings_.front().day;

  if (now_midnight - last_saved_midnight > base::TimeDelta()) {
    // Day changed.
    daily_savings_.push_front({now_midnight, delta});
    if (daily_savings_.size() > kNumOfSavedDailyUptimes)
      daily_savings_.pop_back();
  } else {
    daily_savings_.front().saving += delta;
  }

  RecordSavingsTotal();
  SaveSavingsDaily();
}

uint64_t P3ABandwidthSavingsPermanentState::GetSavingsTotal() const {
  // We record only saving for last N days.
  const base::Time n_days_ago =
      base::Time::Now() - base::TimeDelta::FromDays(kNumOfSavedDailyUptimes);
  return std::accumulate(daily_savings_.begin(), daily_savings_.end(),
                         DailySaving(),
                         [n_days_ago](const auto& u1, const auto& u2) {
                           uint64_t add = 0;
                           // Check only last continious days.
                           if (u2.day > n_days_ago) {
                             add = u2.saving;
                           }
                           return DailySaving{{}, u1.saving + add};
                         })
      .saving;
}

void P3ABandwidthSavingsPermanentState::LoadSavingsDaily() {
  DCHECK(daily_savings_.empty());
  const base::ListValue* list =
      user_prefs_->GetList(prefs::kBandwidthSavedDailyBytes);
  if (!list)
    return;

  for (auto it = list->begin(); it != list->end(); ++it) {
    const base::Value* day = it->FindKey("day");
    const base::Value* saving = it->FindKey("saving");
    if (!day || !saving || !day->is_double() || !saving->is_double())
      continue;
    if (daily_savings_.size() == kNumOfSavedDailyUptimes)
      break;
    daily_savings_.push_back({base::Time::FromDoubleT(day->GetDouble()),
                              (uint64_t)saving->GetDouble()});
  }
}

void P3ABandwidthSavingsPermanentState::SaveSavingsDaily() {
  DCHECK(!daily_savings_.empty());
  DCHECK_LE(daily_savings_.size(), kNumOfSavedDailyUptimes);

  ListPrefUpdate update(user_prefs_, prefs::kBandwidthSavedDailyBytes);
  base::ListValue* list = update.Get();
  list->Clear();
  for (const auto& u : daily_savings_) {
    base::DictionaryValue value;
    value.SetKey("day", base::Value(u.day.ToDoubleT()));
    value.SetKey("saving", base::Value(static_cast<double>(u.saving)));
    list->GetList().push_back(std::move(value));
  }
}

void P3ABandwidthSavingsPermanentState::RecordSavingsTotal() {
  int answer = 0;
  if (daily_savings_.size() == kNumOfSavedDailyUptimes) {
    const uint64_t total =
        static_cast<uint64_t>(GetSavingsTotal() / 1024 / 1024);
    DCHECK_GE(total, 0ULL);
    int counter = 0;
    for (auto* it = BandwidthSavingsBuckets.begin();
         it != BandwidthSavingsBuckets.end(); ++it, ++counter) {
      if (total > *it)
        answer = counter + 1;
    }
  }
  UMA_HISTOGRAM_EXACT_LINEAR(kSavingsDailyUMAHistogramName, answer, 6);
}

}  // namespace brave_perf_predictor
