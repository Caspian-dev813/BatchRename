#ifndef RENAME_ENGINE_H
#define RENAME_ENGINE_H

#include <QFileInfoList>
#include "models.h"
#include "name_generator.h"
#include "backup_manager.h"

class RenameEngine
{
public:
    PreviewItemList buildPreview(const QFileInfoList& files,
                                 int startNumber,
                                 const NameGenerator& generator,
                                 const BackupManager& backupManager);

    OperationResult executeRename(const PreviewItemList& previewList);
    OperationResult removeRenamedFiles(const PreviewItemList& previewList);
};

#endif
