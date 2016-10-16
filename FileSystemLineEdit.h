#ifndef FILE_SYSTEM_LINE_EDIT_H
#define FILE_SYSTEM_LINE_EDIT_H

#include <QLineEdit>
#include <QStringList>
class QCompleter;

class FileSystemLineEdit :
    public QLineEdit
{
public:
    FileSystemLineEdit(QWidget *parent = NULL);
    ~FileSystemLineEdit();

    QStringList paths() const;

    QString directory() const;

    virtual bool event(QEvent* event);

private:
    QCompleter *completer_;
};

#endif // FILE_SYSTEM_LINE_EDIT_H
