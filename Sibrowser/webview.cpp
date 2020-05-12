//Web界面的展示
#include "browser.h"
#include "browserwindow.h"
#include "tabwidget.h"
#include "webpage.h"
#include "webpopupwindow.h"
#include "webview.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QTimer>
#include <QString>

WebView::WebView(QWidget *parent)
    : QWebEngineView(parent),m_loadProgress(0)
{
    //connec函数各参数：发送者、信号、接受者、事件处理函数
    connect(this,&QWebEngineView::loadProgress,[this](int progress){
        m_loadProgress=progress;
    });
    connect(this,&QWebEngineView::loadFinished,[this](bool success){
        if(!success){
            m_loadProgress=0;
        }
    });
    //递交页面终态
    connect(this,&QWebEngineView::renderProcessTerminated,
            [this](QWebEnginePage::RenderProcessTerminationStatus termStatus,int statusCode){
        QString status;
        switch (termStatus) {
        case QWebEnginePage::NormalTerminationStatus:
            status=tr("Render process normal exit");
            break;
        case QWebEnginePage::AbnormalTerminationStatus:
            status=tr("Render process abnormal exit");
            break;
        case QWebEnginePage::CrashedTerminationStatus:
            status=tr("Render process crashed");
            break;
        case QWebEnginePage::KilledTerminationStatus:
            status=tr("Render process killed");
            break;

        }
        QMessageBox::StandardButton btn = QMessageBox::question(window(),status,
                                                                tr("Render process exited with code: %1\n"
                                                                   "Do you want to reload it again?").arg(statusCode));
        if(btn ==QMessageBox::Yes){
            //单词触发执行重新加载
            QTimer::singleShot(0,[this]{reload();});
        }
    });
}

//触发函数
void WebView::createWebActionTrigger(QWebEnginePage *page, QWebEnginePage::WebAction webAction)
{
    QAction *action = page->action(webAction);
    connect(action, &QAction::changed, [this, action, webAction]{
        emit webActionEnableChanged  (webAction, action->isEnabled());
    });
}

//页面的回退、刷新操作
void WebView::setPage(WebPage *page)
{
    createWebActionTrigger(page,QWebEnginePage::Forward);
    createWebActionTrigger(page,QWebEnginePage::Back);
    createWebActionTrigger(page,QWebEnginePage::Reload);
    createWebActionTrigger(page,QWebEnginePage::Stop);
    QWebEngineView::setPage(page);
}

int WebView::loadProgress() const
{
    return m_loadProgress;
}

bool WebView::isWebActionEnabled(QWebEnginePage::WebAction webAction) const
{
    return page()->action(webAction)->isEnabled();
}

QWebEngineView *WebView::createWindow(QWebEnginePage::WebWindowType type){
    switch (type) {
    case QWebEnginePage::WebBrowserTab:{//搜索栏
        BrowserWindow *mainWindow = qobject_cast<BrowserWindow*>(window());
        return mainWindow->tabWidget()
    }

    }
}
WebView::~WebView()
{
}

