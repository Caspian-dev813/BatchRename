#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "models.h"
#include "name_generator.h"
#include "backup_manager.h"
#include "rename_engine.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_browse_clicked();
    void on_btn_execute_clicked();
    void on_btn_restore_clicked();
    void refreshFileCountLabel();

private:
    void syncGeneratorSettings();

    Ui::MainWindow* ui;
    NameGenerator m_nameGenerator;
    BackupManager m_backupManager;
    RenameEngine m_renameEngine;
    PreviewItemList m_previewList;
};

#endif
