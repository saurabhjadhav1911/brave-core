/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include <utility>

#include "bat/ledger/internal/database/database_activity_info.h"
#include "bat/ledger/internal/database/database_migration.h"
#include "bat/ledger/internal/database/database_pending_contribution.h"
#include "bat/ledger/internal/database/database_publisher_info.h"
#include "bat/ledger/internal/database/database_recurring_tip.h"
#include "bat/ledger/internal/database/database_server_publisher_info.h"
#include "bat/ledger/internal/database/database_util.h"
#include "bat/ledger/internal/ledger_impl.h"

using std::placeholders::_1;

namespace braveledger_database {

DatabaseMigration::DatabaseMigration(bat_ledger::LedgerImpl* ledger) :
    ledger_(ledger) {
  activity_info_ = std::make_unique<DatabaseActivityInfo>(ledger_);
  publisher_info_ = std::make_unique<DatabasePublisherInfo>(ledger_);
  recurring_tip_ = std::make_unique<DatabaseRecurringTip>(ledger_);
  server_publisher_info_ =
      std::make_unique<DatabaseServerPublisherInfo>(ledger_);
}

DatabaseMigration::~DatabaseMigration() = default;

void DatabaseMigration::Start(
    const int table_version,
    ledger::ResultCallback callback) {
  const int start_version = table_version + 1;
  auto transaction = ledger::DBTransaction::New();
  int migrated_version = table_version;
  const int target_version = GetCurrentVersion();

  if (target_version == table_version) {
    callback(ledger::Result::LEDGER_OK);
    return;
  }

  for (auto i = start_version; i <= target_version; i++) {
    if (!Migrate(transaction.get(), i)) {
      BLOG(ledger_, ledger::LogLevel::LOG_ERROR) <<
      "DB: Error with MigrateV" << (i - 1) << "toV" << i;
      break;
    }

    BLOG(ledger_, ledger::LogLevel::LOG_INFO) <<
    "DB: Migrated to version " << i;

    migrated_version = i;
  }

  transaction->version = migrated_version;
  transaction->compatible_version = GetCompatibleVersion();
  auto command = ledger::DBCommand::New();
  command->type = ledger::DBCommand::Type::MIGRATE;
  transaction->commands.push_back(std::move(command));

  auto transaction_callback = std::bind(&OnResultCallback,
      _1,
      callback);

  ledger_->RunDBTransaction(std::move(transaction), transaction_callback);
}

bool DatabaseMigration::Migrate(
    ledger::DBTransaction* transaction,
    int version) {
  switch (version) {
    case 1: {
      return MigrateV0toV1(transaction);
    }
    case 2: {
      return MigrateV1toV2(transaction);
    }
    case 3: {
      return MigrateV2toV3(transaction);
    }
    case 4: {
      return MigrateV3toV4(transaction);
    }
    case 5: {
      return MigrateV4toV5(transaction);
    }
    case 6: {
      return MigrateV5toV6(transaction);
    }
    case 7: {
      return MigrateV6toV7(transaction);
    }
    case 8: {
      return MigrateV7toV8(transaction);
    }
    case 9: {
      return MigrateV8toV9(transaction);
    }
    case 10: {
      return MigrateV9toV10(transaction);
    }
    case 11: {
      return MigrateV10toV11(transaction);
    }
    case 12: {
      return MigrateV11toV12(transaction);
    }
    case 13: {
      return MigrateV12toV13(transaction);
    }
    case 14: {
      return MigrateV13toV14(transaction);
    }
    case 15: {
      return MigrateV14toV15(transaction);
    }
    default:
      NOTREACHED();
      return false;
  }
}

bool DatabaseMigration::MigrateV0toV1(
    ledger::DBTransaction* transaction) {
  if (!activity_info_->Migrate(transaction, 1)) {
    return false;
  }

  if (!publisher_info_->Migrate(transaction, 1)) {
    return false;
  }

  return true;
}
bool DatabaseMigration::MigrateV1toV2(
    ledger::DBTransaction* transaction) {
  if (!activity_info_->Migrate(transaction, 2)) {
    return false;
  }

  if (!recurring_tip_->Migrate(transaction, 2)) {
    return false;
  }

  return true;
}

bool DatabaseMigration::MigrateV2toV3(
    ledger::DBTransaction* transaction) {
  if (!pending_contribution_->Migrate(transaction, 3)) {
    return false;
  }

  return true;
}

bool DatabaseMigration::MigrateV3toV4(
    ledger::DBTransaction* transaction) {
  if (!activity_info_->Migrate(transaction, 4)) {
    return false;
  }

  return true;
}

bool DatabaseMigration::MigrateV4toV5(
    ledger::DBTransaction* transaction) {
  if (!activity_info_->Migrate(transaction, 5)) {
    return false;
  }

  return true;
}

bool DatabaseMigration::MigrateV5toV6(
    ledger::DBTransaction* transaction) {
  if (!activity_info_->Migrate(transaction, 6)) {
    return false;
  }

  return true;
}

bool DatabaseMigration::MigrateV6toV7(
    ledger::DBTransaction* transaction) {
  if (!publisher_info_->Migrate(transaction, 7)) {
    return false;
  }

  if (!server_publisher_info_->Migrate(transaction, 7)) {
    return false;
  }

  return true;
}

bool DatabaseMigration::MigrateV7toV8(
    ledger::DBTransaction* transaction) {
  if (!pending_contribution_->Migrate(transaction, 8)) {
    return false;
  }

  return true;
}

bool DatabaseMigration::MigrateV8toV9(
    ledger::DBTransaction* transaction) {
  return true;
}

bool DatabaseMigration::MigrateV9toV10(
    ledger::DBTransaction* transaction) {
  return true;
}

bool DatabaseMigration::MigrateV10toV11(
    ledger::DBTransaction* transaction) {
  return true;
}

bool DatabaseMigration::MigrateV11toV12(
    ledger::DBTransaction* transaction) {
  if (!pending_contribution_->Migrate(transaction, 12)) {
    return false;
  }

  return true;
}

bool DatabaseMigration::MigrateV12toV13(
    ledger::DBTransaction* transaction) {
  return true;
}

bool DatabaseMigration::MigrateV13toV14(
    ledger::DBTransaction* transaction) {
  return true;
}

bool DatabaseMigration::MigrateV14toV15(
    ledger::DBTransaction* transaction) {
  if (!activity_info_->Migrate(transaction, 15)) {
    return false;
  }

  if (!pending_contribution_->Migrate(transaction, 15)) {
    return false;
  }

  if (!recurring_tip_->Migrate(transaction, 15)) {
    return false;
  }

  if (!server_publisher_info_->Migrate(transaction, 15)) {
    return false;
  }

  return true;
}

}  // namespace braveledger_database
