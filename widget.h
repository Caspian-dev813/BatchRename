#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include <QString>
#include <QFileInfo>
#include <QMap>
#include <QFileInfoList>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

struct scan_dir_class{
    int count=0;
    bool valid_status=false;
};

struct backup_files_class
{
    int success = 0;
    int failed = 0;
    QFileInfoList failedFile;
    QStringList errorMsg;
};

struct preview_item_class
{
    QFileInfo sourceFile;
    QString newFileName;
};
using PreviewItemList = QList<preview_item_class>;

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_btn_browse_clicked();
    void on_btn_execute_clicked();
    void on_btn_restore_clicked();

private:
    Ui::Widget *ui;

    scan_dir_class verifyFolder(QString folderPath);
    backup_files_class Backup_files_copyto(const QFileInfoList& allFiles, QString& target_dir);
    void delete_old_files(const PreviewItemList& previewList);

    QMap<QString,QString> m_backupMap;
    QString m_backupFolder;
    PreviewItemList m_previewList;
    backup_files_class m_backupRet;

    bool calcNewName(const QFileInfo& fi, int startNum, QString& outNewName);
    QFileInfoList getFilteredFiles();
    bool fixFolderPermissions(const QString& folderPath);
    bool performRename();
};
#endif // WIDGET_H