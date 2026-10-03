#ifndef BACKUP_MANAGER_H
#define BACKUP_MANAGER_H

#include <QMap>
#include <QString>
#include <QFileInfoList>
#include "models.h"

class BackupManager
{
public:
    BackupResult backup(const QFileInfoList& files, const QString& targetDir);
    OperationResult restore();

    bool contains(const QString& sourcePath) const;
    bool hasBackup() const;

    QString backupFolder() const;
    void setBackupFolder(const QString& folder);

    const QMap<QString, QString>& backupMap() const;

    void reset();

private:
    QMap<QString, QString> m_backupMap;
    QString m_backupFolder;
};

#endif
