#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "file_scanner.h"

#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(690, 700);
    connect(ui->edit_folder, &QLineEdit::textChanged,
            this, &MainWindow::refreshFileCountLabel);
    connect(ui->edit_suffix, &QLineEdit::textChanged,
            this, &MainWindow::refreshFileCountLabel);
    refreshFileCountLabel();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::syncGeneratorSettings()
{
    m_nameGenerator.setPrefix(ui->edit_prefix->text().trimmed());
    m_nameGenerator.setMode(ui->Perfix_mode->isChecked() ? NameGenerator::PrefixMode
                                                         : NameGenerator::SuffixMode);
}

void MainWindow::on_btn_browse_clicked()
{
    QString selectedPath = QFileDialog::getExistingDirectory(this);
    if (!selectedPath.isEmpty())
        ui->edit_folder->setText(selectedPath);
}

void MainWindow::refreshFileCountLabel()
{
    QString folder = ui->edit_folder->text().trimmed();
    QString suffixText = ui->edit_suffix->text().trimmed();

    ScanResult check = FileScanner::verifyFolder(folder);
    if (!check.isValid())
    {
        ui->label_status->setText("Status: 0 fils found");
        return;
    }

    QFileInfoList fileList = FileScanner::collectFilteredFiles(folder, suffixText);
    ui->label_status->setText(QString("Status: %1 fils found").arg(fileList.size()));
}


void MainWindow::on_btn_execute_clicked()
{
    if (FileScanner::rootFolderExists("_backup_rename"))
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Found _backup_rename folder in the root directory of current disk.\nIt may conflict with program operation.\nDo you want to continue?");
        msgBox.addButton("Continue", QMessageBox::YesRole);
        msgBox.setDefaultButton(msgBox.addButton("Cancel", QMessageBox::NoRole));
        msgBox.exec();
        if (msgBox.clickedButton() == msgBox.buttons()[1])
            return;
    }

    QString folder = ui->edit_folder->text().trimmed();
    QString suffixText = ui->edit_suffix->text().trimmed();

    ScanResult folderCheck = FileScanner::verifyFolder(folder);
    if (!folderCheck.isValid())
    {
        if (folderCheck.status == ScanResult::IsFile)
            QMessageBox::critical(this, "Error", "Target is a file.");
        else
            QMessageBox::critical(this, "Error", "Target folder does not exist.");
        return;
    }

    QFileInfoList fileList = FileScanner::collectFilteredFiles(folder, suffixText);
    if (fileList.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "No files to process");
        return;
    }

    bool numberOk = false;
    int startNumber = ui->lineEdit->text().toInt(&numberOk);
    if (!numberOk || startNumber < 1)
    {
        QMessageBox::warning(this, "Warning", "Please input valid start number >=1");
        return;
    }

    QString existingBackup = m_backupManager.backupFolder();
    if (!existingBackup.isEmpty() && QDir(existingBackup).exists())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Question");
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText("A non-empty backup folder already exists.\n\nIt must be removed before the next rename task can proceed.\n\nDo you agree?");
        msgBox.addButton("Agree", QMessageBox::YesRole);
        msgBox.setDefaultButton(msgBox.addButton("Disagree", QMessageBox::NoRole));
        msgBox.exec();
        if (msgBox.clickedButton() == msgBox.buttons()[1])
            return;
        if (!QDir(existingBackup).removeRecursively())
        {
            QMessageBox::critical(this, "Error", "Cannot delete old backup folder! Files may be locked.");
            return;
        }
    }

    m_backupManager.reset();
    m_previewList.clear();

    QString backupFolder = QDir(folder).filePath("_backup_rename");
    m_backupManager.setBackupFolder(backupFolder);
    BackupResult backupResult = m_backupManager.backup(fileList, backupFolder);

    if (backupResult.failed > 0)
        QMessageBox::critical(this, "Backup failed", backupResult.errorMessages.join("\n"));
    if (backupResult.success == 0)
        return;

    syncGeneratorSettings();
    m_previewList = m_renameEngine.buildPreview(fileList, startNumber, m_nameGenerator, m_backupManager);

    OperationResult renameResult = m_renameEngine.executeRename(m_previewList);
    QMessageBox::information(this, "Complete",
        QString("Rename ok:%1 fail:%2\nYou must manually delete the backup folder; the software will not clean it automatically.")
            .arg(renameResult.success).arg(renameResult.failed));
}

void MainWindow::on_btn_restore_clicked()
{
    if (!m_backupManager.hasBackup() || m_previewList.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "No backup data");
        return;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Question");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("Are you sure you want to restore all files from backup?");
    msgBox.addButton("Ok", QMessageBox::YesRole);
    msgBox.setDefaultButton(msgBox.addButton("No", QMessageBox::NoRole));
    msgBox.exec();
    if (msgBox.clickedButton() == msgBox.buttons()[1])
        return;

    OperationResult restoreResult = m_backupManager.restore();
    if (restoreResult.failed > 0)
    {
        QMessageBox::critical(this, "Restore Warning",
            QString("Restore ok:%1 fail:%2\nSome files restore failed.")
                .arg(restoreResult.success).arg(restoreResult.failed));
        return;
    }

    OperationResult deleteResult = m_renameEngine.removeRenamedFiles(m_previewList);
    QMessageBox::information(this, "Delete renamed files",
        QString("ok:%1 fail:%2").arg(deleteResult.success).arg(deleteResult.failed));

    m_backupManager.reset();
    m_previewList.clear();
    QMessageBox::information(this, "Restore finish",
        QString("Restore ok:%1 fail:%2\nYou must manually delete the backup folder; the software will not clean it automatically.")
            .arg(restoreResult.success).arg(restoreResult.failed));
}
