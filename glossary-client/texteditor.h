#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QTextEdit>
#include <QMouseEvent>
#include <QString>
#include <QWidget>
#include <QPoint>
#include <QCursor>

class TextEditor : public QTextEdit
{
    Q_OBJECT
public:
    explicit TextEditor(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
signals:
    void linkClicked(QString uri);
private:
    QCursor m_cursor;
    bool m_isOnAnchor;
};

#endif // TEXTEDITOR_H
