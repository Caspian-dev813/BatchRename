#include "backup_manager.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>

BackupResult BackupManager::backup(const QFileInfoList& files, const QString& targetDir)
{
    BackupResult result;
    QDir dir;
    if (!dir.mkpath(targetDir))
    {
        result.failed = files.size();
        result.errorMessages.append("Cannot create backup folder.");
        return result;
    }
    for (const QFileInfo& fi : files)
    {
        QString source = fi.absoluteFilePath();
        QString destination = QDir(targetDir).filePath(fi.fileName());
        if (QFileInfo::exists(destination))
        {
            result.failed++;
            result.failedFiles.append(fi);
            result.errorMessages.append(QString("%1 : Target backup file already exists").arg(fi.fileName()));
            continue;
        }
        if (QFile::copy(source, destination))
        {
            result.success++;
            m_backupMap.insert(source, destination);
        }
        else
        {
            result.failed++;
            result.failedFiles.append(fi);
            result.errorMessages.append(QString("%1 : %2").arg(fi.fileName(), QFile(source).errorString()));
        }
    }
    return result;
}

OperationResult BackupManager::restore()
{
    OperationResult result;
    for (auto it = m_backupMap.begin(); it != m_backupMap.end(); ++it)
    {
        const QString originalPath = it.key();
        const QString backupPath = it.value();
        if (QFileInfo::exists(originalPath))
        {
            result.failed++;
            continue;
        }
        if (QFile::copy(backupPath, originalPath))
            result.success++;
        else
            result.failed++;
    }
    return result;
}

bool BackupManager::contains(const QString& sourcePath) const
{
    return m_backupMap.contains(sourcePath);
}

bool BackupManager::hasBackup() const
{
    return !m_backupMap.isEmpty();
}

QString BackupManager::backupFolder() const
{
    return m_backupFolder;
}

void BackupManager::setBackupFolder(const QString& folder)
{
    m_backupFolder = folder;
}

const QMap<QString, QString>& BackupManager::backupMap() const
{
    return m_backupMap;
}

void BackupManager::reset()
{
    m_backupMap.clear();
    m_backupFolder.clear();
}
