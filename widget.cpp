#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QMessageBox>
#include <QCoreApplication>
#include <QProcess>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_backupFolder.clear();
    m_backupMap.clear();
    m_previewList.clear();
}

Widget::~Widget()
{
    if(!m_backupFolder.isEmpty())
    {
        QDir dir(m_backupFolder);
        dir.removeRecursively();
    }
    delete ui;
}

scan_dir_class Widget::verifyFolder(QString folderPath)
{
    scan_dir_class res;
    QFileInfo info(folderPath);
    if (info.exists())
    {
        if (info.isDir())
        {
            QDir dir(folderPath);
            dir.setFilter(QDir::Files);
            QFileInfoList list = dir.entryInfoList();
            res.count = list.size();
            res.valid_status = true;
        }
        else
        {
            QMessageBox::critical(this, "Error", "Target is a file.");
            res.valid_status = false;
        }
    }
    else
    {
        QMessageBox::critical(this, "Error", "Target folder does not exist.");
        res.valid_status = false;
    }
    return res;
}

static QStringList Get_filter_suffix(QString str_suffix)
{
    QStringList res;
    int start = 0;
    for(int i = 0; i < str_suffix.size(); ++i)
    {
        if(str_suffix[i] == QChar(';'))
        {
            QString item = str_suffix.mid(start, i - start).toLower();
            if(!item.isEmpty())
                res << item;
            start = i + 1;
        }
    }
    QString last = str_suffix.mid(start).toLower();
    if(!last.isEmpty())
        res << last;
    return res;
}

static QFileInfoList FilterBySuffix(const QFileInfoList& allFiles, const QStringList& suffixFile)
{
    QFileInfoList out;
    if(suffixFile.isEmpty())
    {
        return allFiles;
    }
    for(const QFileInfo& fi : allFiles)
    {
        QString suf = fi.suffix().toLower();
        if(suffixFile.contains(suf))
        {
            out.append(fi);
        }
    }
    return out;
}

QFileInfoList Widget::getFilteredFiles()
{
    QString folder = ui->edit_folder->text().trimmed();
    scan_dir_class dirCheck = verifyFolder(folder);
    if(!dirCheck.valid_status)
    {
        return {};
    }
    QDir dir(folder);
    dir.setFilter(QDir::Files);
    QFileInfoList all = dir.entryInfoList();
    QString suffixText = ui->edit_suffix->text().trimmed();
    QStringList suffixList = Get_filter_suffix(suffixText);
    return FilterBySuffix(all, suffixList);
}

bool Widget::calcNewName(const QFileInfo& fi, int startNum, QString& outNewName)
{
    QString base = fi.completeBaseName();
    QString ext = fi.suffix();
    QString prefixText = ui->edit_prefix->text().trimmed();
    QString numStr = QString::number(startNum,10).rightJustified(3,'0');
    QString newBase;
    if(ui->Perfix_mode->isChecked())
    {
        newBase = prefixText + numStr;
    }
    else
    {
        newBase = numStr + prefixText;
    }
    if(ext.isEmpty())
    {
        outNewName = newBase;
    }
    else
    {
        outNewName = newBase + "." + ext;
    }
    return true;
}

backup_files_class Widget::Backup_files_copyto(const QFileInfoList& allFiles, QString& target_dir)
{
    backup_files_class res;
    QDir dir;
    if(QDir(target_dir).exists())
    {
        QDir backupDir(target_dir);
        backupDir.removeRecursively();
    }
    QFileInfo parentInfo(QFileInfo(target_dir).absolutePath());
    bool parentWritable = parentInfo.isWritable();
    if(!dir.mkpath(target_dir))
    {
        QString fallbackDir = QDir(QDir::tempPath()).filePath(
            QString("_backup_rename_%1").arg(QCoreApplication::applicationPid()));
        if(QDir(fallbackDir).exists())
        {
            QDir(fallbackDir).removeRecursively();
        }
        if(dir.mkpath(fallbackDir))
        {
            target_dir = fallbackDir;
        }
        else
        {
            res.failed = allFiles.size();
            QString detail;
            if(!parentInfo.exists())
                detail = QString("Parent folder does not exist: %1").arg(parentInfo.absoluteFilePath());
            else if(!parentWritable)
                detail = QString("No write permission to parent folder:\n%1\n\n"
                                 "On macOS, if the folder is in Desktop/Documents/Downloads,\n"
                                 "please grant Full Disk Access to this app in:\n"
                                 "System Settings -> Privacy & Security -> Full Disk Access")
                             .arg(parentInfo.absoluteFilePath());
            else
                detail = QString("Cannot create folder:\n%1").arg(target_dir);
            res.errorMsg.append(QString("Cannot create backup folder.\n%1").arg(detail));
            return res;
        }
    }
    for(const QFileInfo& fi : allFiles)
    {
        QString src = fi.absoluteFilePath();
        QString dst = QDir(target_dir).filePath(fi.fileName());
        if(QFile::exists(dst))
        {
            QFile::remove(dst);
        }
        QFile fsrc(src);
        bool ok = fsrc.copy(dst);
        if(ok)
        {
            res.success++;
            m_backupMap.insert(src, dst);
        }
        else
        {
            res.failed++;
            res.failedFile.append(fi);
            res.errorMsg.append(QString("%1 : %2").arg(fi.fileName(), fsrc.errorString()));
        }
    }
    return res;
}

void Widget::delete_old_files(const PreviewItemList& previewList)
{
    int ok=0,failed=0;
    for(const auto& item : previewList)
    {
        QString fullPath = QDir(item.sourceFile.absolutePath()).filePath(item.newFileName);
        bool delOk = QFile::remove(fullPath);
        if(!delOk)
        {
            QProcess proc;
            proc.start("/bin/rm", QStringList() << "-f" << fullPath);
            delOk = proc.waitForFinished(10000) && proc.exitCode() == 0;
        }
        if(delOk)
            ok++;
        else
            failed++;
    }
    QMessageBox::information(this,"Delete renamed files",QString("ok:%1 fail:%2").arg(ok).arg(failed));
}

void Widget::on_btn_browse_clicked()
{
    QString SelectPath = QFileDialog::getExistingDirectory(this);
    if(!SelectPath.isEmpty())
        ui->edit_folder->setText(SelectPath);
}

void Widget::on_btn_execute_clicked()
{
    QFileInfoList fileList = getFilteredFiles();
    if(fileList.isEmpty())
    {
        QMessageBox::warning(this,"Warning","No files to process");
        return;
    }
    bool okNum;
    int startIdx = ui->lineEdit->text().toInt(&okNum);
    if(!okNum || startIdx < 1)
    {
        QMessageBox::warning(this,"Warning","Please input valid start number >=1");
        return;
    }
    QString srcFolder = ui->edit_folder->text().trimmed();
    m_backupFolder = QDir(srcFolder).filePath("_backup_rename");
    m_backupMap.clear();
    m_backupRet = Backup_files_copyto(fileList,m_backupFolder);
    if(m_backupRet.failed>0)
    {
        QMessageBox::critical(this,"Backup failed",m_backupRet.errorMsg.join("\n"));
    }
    if(m_backupRet.success <=0)
    {
        return;
    }
    m_previewList.clear();
    int curNum = startIdx;
    for(const QFileInfo& fi : fileList)
    {
        if(!m_backupMap.contains(fi.absoluteFilePath()))
        {
            continue;
        }
        QString newName;
        calcNewName(fi,curNum,newName);
        preview_item_class item;
        item.sourceFile = fi;
        item.newFileName = newName;
        m_previewList.append(item);
        curNum++;
    }
    bool allOk = performRename();
    if(!allOk)
    {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Permission Denied",
            "Rename failed due to permission denied.\n"
            "The folder/files may be owned by another user (e.g. root).\n\n"
            "Try to fix permissions automatically?\n"
            "(You will be asked to enter your password.)",
            QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes)
        {
            if(fixFolderPermissions(srcFolder))
            {
                performRename();
            }
            else
            {
                QMessageBox::warning(this, "Failed",
                    "Permission fix failed or cancelled.");
            }
        }
    }
}

bool Widget::performRename()
{
    int renameOk=0,renameFail=0;
    QStringList renameErrors;
    bool permissionDenied = false;
    for(auto &item : m_previewList)
    {
        QString srcPath = item.sourceFile.absoluteFilePath();
        QString targetPath = QDir(item.sourceFile.absolutePath()).filePath(item.newFileName);
        if(!QFile::exists(srcPath))
        {
            renameOk++;
            continue;
        }
        QFile f(srcPath);
        if(f.rename(targetPath))
        {
            renameOk++;
            continue;
        }
        QString err = f.errorString();
        if(err.contains("Permission", Qt::CaseInsensitive) ||
           err.contains("denied", Qt::CaseInsensitive))
        {
            permissionDenied = true;
            QProcess proc;
            proc.start("/bin/mv", QStringList() << srcPath << targetPath);
            if(proc.waitForFinished(10000) && proc.exitCode() == 0)
            {
                renameOk++;
                continue;
            }
        }
        renameFail++;
        renameErrors.append(QString("%1 -> %2 : %3")
            .arg(item.sourceFile.fileName(), item.newFileName, err));
    }
    QString msg = QString("Rename ok:%1 fail:%2").arg(renameOk).arg(renameFail);
    if(!renameErrors.isEmpty())
    {
        if(permissionDenied && renameFail == m_previewList.size())
        {
            msg += "\n\nAll renames failed due to permission denied.";
        }
        else
        {
            msg += "\n\n" + renameErrors.join("\n");
        }
    }
    QMessageBox::information(this,"Complete",msg);
    return renameFail == 0;
}

bool Widget::fixFolderPermissions(const QString& folderPath)
{
    QString user = qEnvironmentVariable("USER");
    QString group = "staff";
    QString escapedPath = folderPath;
    escapedPath.replace("\\", "\\\\");
    escapedPath.replace("\"", "\\\"");
    QString script = QString(
        "set fp to \"%1\"\n"
        "do shell script \"chown -R %2:%3 \" & quoted form of fp "
        "with administrator privileges")
        .arg(escapedPath, user, group);
    QProcess proc;
    proc.start("/usr/bin/osascript", QStringList() << "-e" << script);
    if(!proc.waitForFinished(120000))
    {
        return false;
    }
    return proc.exitCode() == 0;
}

void Widget::on_btn_restore_clicked()
{
    if(m_backupMap.isEmpty() || m_previewList.isEmpty())
    {
        QMessageBox::warning(this,"Warning","No backup data");
        return;
    }
    delete_old_files(m_previewList);
    int restoreOk=0,restoreFail=0;
    for(auto iter = m_backupMap.begin();iter != m_backupMap.end();++iter)
    {
        QString originalPath = iter.key();
        QString backupPath = iter.value();
        QFile f(backupPath);
        if(f.copy(originalPath))
        {
            restoreOk++;
            continue;
        }
        if(QFile::exists(originalPath))
        {
            QFile::remove(originalPath);
        }
        QProcess proc;
        proc.start("/bin/cp", QStringList() << backupPath << originalPath);
        if(proc.waitForFinished(10000) && proc.exitCode() == 0)
        {
            restoreOk++;
        }
        else
        {
            restoreFail++;
        }
    }
    m_backupMap.clear();
    m_previewList.clear();
    QMessageBox::information(this,"Restore finish",QString("Restore ok:%1 fail:%2").arg(restoreOk).arg(restoreFail));
}