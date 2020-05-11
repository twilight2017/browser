#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebEngineView>
#include <QIcon>

class WebPage;

class WebView : public QWebEngineView
{
    Q_OBJECT

public:
    WebView(QWidget *parent = nullptr);
    void setPage(WebPage *page);

    int loadProgress() const;
    bool isWebActionEnabled(QWebEnginePage::WebAction webAction) const;

    ~WebView();
protected:
    void contextMenuEvent(QContextMenuEvent *event)override;
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type)override;
signals:
    void webActionEnableChanged(QWebEnginePage::WebWindowType type)override;
private:
    void createWebActionTrigger(QWebEnginePage *page,QWebEnginePage::WebAction );
private:
    int m_loadProgress;
};
#endif // WEBVIEW_H
