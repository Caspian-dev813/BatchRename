#include "file_scanner.h"
#include <QDir>
#include <QFileInfo>
#include <QApplication>

ScanResult FileScanner::verifyFolder(const QString& folderPath)
{
    ScanResult result;
    QFileInfo info(folderPath);
    if (!info.exists())
    {
        result.status = ScanResult::NotExist;
        return result;
    }
    if (!info.isDir())
    {
        result.status = ScanResult::IsFile;
        return result;
    }
    QDir dir(folderPath);
    dir.setFilter(QDir::Files);
    result.status = ScanResult::Ok;
    result.count = dir.entryInfoList().size();
    return result;
}

QFileInfoList FileScanner::scanFiles(const QString& folderPath)
{
    QDir dir(folderPath);
    dir.setFilter(QDir::Files);
    return dir.entryInfoList();
}

QStringList FileScanner::parseSuffixList(const QString& suffixText)
{
    QStringList result;
    int start = 0;
    for (int i = 0; i < suffixText.size(); ++i)
    {
        if (suffixText[i] == QChar(';'))
        {
            QString item = suffixText.mid(start, i - start).toLower();
            if (!item.isEmpty())
                result << item;
            start = i + 1;
        }
    }
    QString last = suffixText.mid(start).toLower();
    if (!last.isEmpty())
        result << last;
    return result;
}

QFileInfoList FileScanner::filterBySuffix(const QFileInfoList& files, const QStringList& suffixes)
{
    if (suffixes.isEmpty())
    {
        return files;
    }
    QFileInfoList out;
    for (const QFileInfo& fi : files)
    {
        if (suffixes.contains(fi.suffix().toLower()))
            out.append(fi);
    }
    return out;
}

QFileInfoList FileScanner::collectFilteredFiles(const QString& folderPath, const QString& suffixText)
{
    ScanResult check = verifyFolder(folderPath);
    if (!check.isValid())
    {
        return {};
    }
    QFileInfoList all = scanFiles(folderPath);
    return filterBySuffix(all, parseSuffixList(suffixText));
}

bool FileScanner::rootFolderExists(const QString& folderName)
{
    QFileInfo exeInfo(qApp->applicationFilePath());
    QString driveRoot = exeInfo.absolutePath().left(2) + "/";
    QString targetPath = QDir(driveRoot).filePath(folderName);
    QFileInfo info(targetPath);
    return info.exists() && info.isDir();
}
