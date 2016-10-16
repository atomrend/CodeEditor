#ifndef FILEINFODIALOG_H
#define FILEINFODIALOG_H

#include <QDialog>
class QTableWidget;

class FileInfoDialog :
    public QDialog
{
    Q_OBJECT
public:
    FileInfoDialog(QWidget* parent = 0);

    static FileInfoDialog& get(QWidget* parent = 0);

public slots:
    int exec();

private slots:
    void onApply();
    void onBrowse();

protected:
    QTableWidget *table_;

    static FileInfoDialog *singleton_;
};

#endif // FILEINFODIALOG_H
