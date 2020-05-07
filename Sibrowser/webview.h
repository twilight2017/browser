#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWidget>

class WebView : public QWidget
{
    Q_OBJECT

public:
    WebView(QWidget *parent = nullptr);
    ~WebView();
};
#endif // WEBVIEW_H
