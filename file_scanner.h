#ifndef FILE_SCANNER_H
#define FILE_SCANNER_H

#include <QString>
#include <QStringList>
#include <QFileInfoList>
#include "models.h"

class FileScanner
{
public:
    static ScanResult verifyFolder(const QString& folderPath);
    static QFileInfoList scanFiles(const QString& folderPath);
    static QStringList parseSuffixList(const QString& suffixText);
    static QFileInfoList filterBySuffix(const QFileInfoList& files, const QStringList& suffixes);
    static QFileInfoList collectFilteredFiles(const QString& folderPath, const QString& suffixText);
    static bool rootFolderExists(const QString& folderName);
};

#endif
