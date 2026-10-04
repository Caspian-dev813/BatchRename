#include "rename_engine.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>

PreviewItemList RenameEngine::buildPreview(const QFileInfoList& files,
                                           int startNumber,
                                           const NameGenerator& generator,
                                           const BackupManager& backupManager)
{
    PreviewItemList preview;
    int currentNumber = startNumber;
    for (const QFileInfo& fi : files)
    {
        if (!backupManager.contains(fi.absoluteFilePath()))
            continue;
        PreviewItem item;
        item.sourceFile = fi;
        item.newFileName = generator.generate(fi, currentNumber);
        preview.append(item);
        currentNumber++;
    }
    return preview;
}

OperationResult RenameEngine::executeRename(const PreviewItemList& previewList)
{
    OperationResult result;
    for (const PreviewItem& item : previewList)
    {
        QString sourcePath = item.sourceFile.absoluteFilePath();
        QString targetPath = QDir(item.sourceFile.absolutePath()).filePath(item.newFileName);
        if (QFileInfo::exists(targetPath))
        {
            result.failed++;
            continue;
        }
        QFile file(sourcePath);
        if (file.rename(targetPath))
            result.success++;
        else
            result.failed++;
    }
    return result;
}

OperationResult RenameEngine::removeRenamedFiles(const PreviewItemList& previewList)
{
    OperationResult result;
    for (const PreviewItem& item : previewList)
    {
        QString fullPath = QDir(item.sourceFile.absolutePath()).filePath(item.newFileName);
        if (QFile::remove(fullPath))
            result.success++;
        else
            result.failed++;
    }
    return result;
}
