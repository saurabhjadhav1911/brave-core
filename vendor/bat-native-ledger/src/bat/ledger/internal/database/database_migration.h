/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_DATABASE_DATABASE_MIGRATION_H_
#define BRAVELEDGER_DATABASE_DATABASE_MIGRATION_H_

#include <memory>

#include "bat/ledger/ledger.h"

namespace bat_ledger {
class LedgerImpl;
}

namespace braveledger_database {

class DatabaseActivityInfo;
class DatabasePendingContribution;
class DatabasePublisherInfo;
class DatabaseRecurringTip;
class DatabaseServerPublisherInfo;

class DatabaseMigration {
 public:
  explicit DatabaseMigration(bat_ledger::LedgerImpl* ledger);
  ~DatabaseMigration();

  void Start(
      const int table_version,
      ledger::ResultCallback callback);

 private:
  bool Migrate(ledger::DBTransaction* transaction, int version);

  bool MigrateV0toV1(ledger::DBTransaction* transaction);

  bool MigrateV1toV2(ledger::DBTransaction* transaction);

  bool MigrateV2toV3(ledger::DBTransaction* transaction);

  bool MigrateV3toV4(ledger::DBTransaction* transaction);

  bool MigrateV4toV5(ledger::DBTransaction* transaction);

  bool MigrateV5toV6(ledger::DBTransaction* transaction);

  bool MigrateV6toV7(ledger::DBTransaction* transaction);

  bool MigrateV7toV8(ledger::DBTransaction* transaction);

  bool MigrateV8toV9(ledger::DBTransaction* transaction);

  bool MigrateV9toV10(ledger::DBTransaction* transaction);

  bool MigrateV10toV11(ledger::DBTransaction* transaction);

  bool MigrateV11toV12(ledger::DBTransaction* transaction);

  bool MigrateV12toV13(ledger::DBTransaction* transaction);

  bool MigrateV13toV14(ledger::DBTransaction* transaction);

  bool MigrateV14toV15(ledger::DBTransaction* transaction);

  std::unique_ptr<DatabaseActivityInfo> activity_info_;
  std::unique_ptr<DatabasePendingContribution> pending_contribution_;
  std::unique_ptr<DatabasePublisherInfo> publisher_info_;
  std::unique_ptr<DatabaseRecurringTip> recurring_tip_;
  std::unique_ptr<DatabaseServerPublisherInfo> server_publisher_info_;
  bat_ledger::LedgerImpl* ledger_;  // NOT OWNED
};

}  // namespace braveledger_database
#endif  // BRAVELEDGER_DATABASE_DATABASE_MIGRATION_H_
