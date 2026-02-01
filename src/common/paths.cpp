// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2024 jo-hannes <jo-hannes@dev-urandom.de>

#include "paths.h"

#include "jsonhelper.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

// static members
QString Paths::pathDataDir;

QString Paths::dataDir() {
  if (!pathDataDir.isEmpty()) {
    return pathDataDir;
  }

  QStringList paths;
  paths.append(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
  paths.append(QCoreApplication::applicationDirPath());
  paths.append(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));

  // first search for path.json
  for (const auto& p : paths) {
    if (QFile::exists(p + "/path.json")) {
      pathDataDir = loadPathJson(p);
      if (!pathDataDir.isEmpty()) {
        return pathDataDir;
      }
    }
  }
  // check for old config without path.json
  for (const auto& p : paths) {
    if (QFile::exists(p + "/settings.json")) {
      savePathJson(p);
      pathDataDir = p;
      return pathDataDir;
    }
  }
  // Nothing found create new path.json
  for (const auto& p : paths) {
    if (QDir().mkpath(p) && savePathJson(p)) {
      pathDataDir = p;
      return pathDataDir;
    }
  }

  return paths.at(0);
}

QString Paths::loadPathJson(QString dir) {
  QJsonObject json = JsonHelper::loadFile(dir + "/path.json");
  QString path = json["dataDir"].toString();
  if (path.isEmpty()) {
    return path;
  }
  // check for relative path
  const QFileInfo fi(path);
  if (fi.isRelative()) {
    // convert to absolute path
    const QDir helperPath(dir + "/" + path);
    return helperPath.absolutePath();
  }
  return path;
}

bool Paths::savePathJson(QString dir) {
  QJsonObject json;
  json["dataDir"] = ".";
  return JsonHelper::saveFile(dir + "/path.json", json);
}
