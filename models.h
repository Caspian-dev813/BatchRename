#ifndef MODELS_H
#define MODELS_H

#include <QString>
#include <QList>
#include <QFileInfo>
#include <QFileInfoList>

struct ScanResult
{
    enum Status
    {
        Ok,
        NotExist,
        IsFile
    };

    Status status = NotExist;
    int count = 0;

    bool isValid() const { return status == Ok; }
};

struct BackupResult
{
    int success = 0;
    int failed = 0;
    QFileInfoList failedFiles;
    QStringList errorMessages;
};

struct OperationResult
{
    int success = 0;
    int failed = 0;
};

struct PreviewItem
{
    QFileInfo sourceFile;
    QString newFileName;
};

using PreviewItemList = QList<PreviewItem>;

#endif
