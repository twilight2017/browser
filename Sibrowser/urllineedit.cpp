﻿#include "urllineedit.h"
#include <QToolButton>
#include <QUrl>

UrlLineEdit::UrlLineEdit(QWidget *parent)
    :QLineEdit(parent)
    ,m_favButton(new QToolButton(this))
    ,m_clearButton(new QToolButton(this))
{
    m_clearButton->setIcon(QIcon(QStringLiteral(":/data/closetab.png")));
    m_clearButton->setVisible(false);
    m_clearButton->setCursor(Qt::ArrowCursor);
    QString style(QStringLiteral("QToolButton { border: none; padding: 1px; }"));
    m_clearButton->setStyleSheet(style);
    m_favButton->setStyleSheet(style);
    setStyleSheet(QStringLiteral("QLineEdit { padding-left: %1px; padding-right: %2px;  } ")
                  .arg(m_clearButton->sizeHint().width())
                  .arg(m_favButton->sizeHint().width()));
    int minIconHeight = qMax(m_favButton->sizeHint().height(),m_clearButton->sizeHint().height());
    setMinimumSize(minimumSizeHint().width() +
                   m_favButton->sizeHint().width() +
                   m_clearButton->sizeHint().width(),
                   qMax(minimumSizeHint().height(), minIconHeight));
    connect(m_clearButton, &QToolButton::clicked, this, &QLineEdit::clear);
    connect(this, &QLineEdit::textChanged, [this](const QString &text) {
        m_clearButton->setVisible(!text.isEmpty() && !isReadOnly());
    });
}

//get url from user
QUrl UrlLineEdit::url() const
{
    return QUrl::fromUserInput(text());
}

//set url visiable
void UrlLineEdit::setUrl(const QUrl &url)
{
    setText(url.toString());
    setCursorPosition(0);
}

//set fav icon
void UrlLineEdit::setFavIcon(const QIcon &icon)
{
    QPixmap pixmap = icon.pixmap(16,16);
    m_favButton->setIcon(pixmap);
}

//resize event
void UrlLineEdit::resizeEvent(QResizeEvent *event)
{
    QLineEdit::resizeEvent(event);//调用库已有函数
    QSize clearButtonSize = m_clearButton->sizeHint();
    m_clearButton->move(rect().right() - clearButtonSize.width(),
                        (rect().bottom() - clearButtonSize.height()) / 2);
    m_favButton->move(rect().left(), (rect().bottom() - m_favButton->sizeHint().height()) / 2);
}
