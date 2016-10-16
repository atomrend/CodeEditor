#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <QVector>
#include <QWidget>
class Editor;
class QComboBox;
class QLabel;

class StatusBar :
    public QWidget
{
    Q_OBJECT
public:
    StatusBar(Editor* editor, QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~StatusBar();

    void setLineNumber(unsigned int val);
    void setColumnNumber(unsigned int val);

    void updateDocumentList();

    static int count();

    static StatusBar* statusBar(int index);

    static void updateAllStatusBars();

signals:
    void documentSelected(int index);

public slots:
    void changeSelectedDocument(int index);

protected:
    void updatePosition();

protected:
    unsigned int lineNum_;
    unsigned int colNum_;
    Editor *editor_;
    QLabel *positionLabel_;
    QComboBox *documentSelector_;

    static QVector<StatusBar*> statusBars_;
};

#endif // STATUS_BAR_H
