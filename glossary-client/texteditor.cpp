#include "texteditor.h"

TextEditor::TextEditor(QWidget *parent) :
    QTextEdit(parent)
{
    viewport()->setMouseTracking(true);
    m_isOnAnchor=false;
    m_cursor=viewport()->cursor();
}

void TextEditor::mousePressEvent(QMouseEvent *e)
{
    QTextEdit::mousePressEvent(e);
    if(!isReadOnly() && e->modifiers()!=Qt::CTRL)
    {
        e->accept();
        return;
    }
    if(e->button()==Qt::LeftButton)
    {
        QPoint pos=e->pos();
        QString anchor=anchorAt(pos);
        if(anchor.length()>0)
            emit linkClicked(anchor);
    }
    e->accept();
}

void TextEditor::mouseMoveEvent(QMouseEvent *e)
{
    QTextEdit::mouseMoveEvent(e);
    QPoint pos=e->pos();
    QString anchor=anchorAt(pos);

    if(anchor.length()>0)
    {
        if(!isReadOnly() && e->modifiers()!=Qt::CTRL)
        {
            e->accept();
            return;
        }
        if(!m_isOnAnchor)
            m_cursor=viewport()->cursor();
        viewport()->setCursor(Qt::PointingHandCursor);
        m_isOnAnchor=true;
    }
    else
    {
        if(m_isOnAnchor)
            viewport()->setCursor(m_cursor);
        else
            m_cursor=viewport()->cursor();
        m_isOnAnchor=false;
    }
    e->accept();
}
