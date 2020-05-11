//用于控制弹出窗口的类
#ifndef WEBPOPUPWINDOW_H
#define WEBPOPUPWINDOW_H

#include <QWidget>

class QWebEngineProfile;
class QWebEngine;

class WebView;
class UrlLineEdit;

class WebPopupWindow : public QWidget
{
    Q_OBJECT
public:
    explicit WebPopupWindow(QWidget *parent = nullptr);

signals:

};

#endif // WEBPOPUPWINDOW_H
