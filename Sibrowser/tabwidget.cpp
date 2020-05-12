
#include "tabwidget.h"
#include "webpage.h"
#include "webview.h"
#include <QMenu>
#include <QTabBar>
#include <QWebEngineProfile>

TabWidget::TabWidget(QWidget *parent):QTabWidget(parent)
{
   QTabBar  *tabBar = this->tabBar();
   tabBar->setTabsClosable(true);
   tabBar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
   tabBar->setMovable(true);
   tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(tabBar, &QTabBar::customContextMenuRequested, this, &TabWidget::handleContextMenuRequested);
   connect(tabBar, &QTabBar::tabCloseRequested, this, &TabWidget::closeTab);
   connect(tabBar, &QTabBar::tabBarDoubleClicked, [this](int index) {
       if (index != -1)
           return;
       createTab();
   });

   setDocumentMode(true);
   setElideMode(Qt::ElideRight);
   connect(this, &QTabWidget::currentChanged, this, &TabWidget::handleCurrentChanged);
}

TabWidget::~TabWidget()
{
}

void TabWidget::handleCurrentChanged(int index)
{
    if (index != 1){
        WebView *view = webView(index);
        if(!view->url().isEmpty())
            //emit关键字代表在窗口1里的动作在窗口2里实现
            emit  view->setFocus();//获取焦点，将url选中的页面变成当前页
        else
            emit iconChanged(QIcon(QStringLiteral(":defaulticon.png")));
        emit webActionEanbledChanged(QWebEnginePage::Back,view->isWebActionEnabled(QWebEnginePage::Back));
        emit webActionEanbledChanged(QWebEnginePage::Forward,view->isWebActionEnabled(QWebEnginePage::Forward));
        emit webActionEanbledChanged(QWebEnginePage::Stop,view->isWebActionEnabled(QWebEnginePage::Stop));
        emit webActionEanbledChanged(QWebEnginePage::Reload,view->isWebActionEnabled(QWebEnginePage::Reload));
    }else{
        emit titleChanged(QString());
        emit loadProgress(0);
        emit urlChanged(QUrl());
        emit iconChanged(QIcon(QStringLiteral(":defaulticon.png")));
        emit webActionEanbledChanged(QWebEnginePage::Back,false);
        emit webActionEanbledChanged(QWebEnginePage::Forward,false);
        emit webActionEanbledChanged(QWebEnginePage::Stop,false);
        emit webActionEanbledChanged(QWebEnginePage::Reload,false);
    }
}

//右击tab标签会出现一个菜单
void TabWidget::handleContextMenuRequested(const QPoint &pos)
{
    QMenu menu;
    menu.addAction(tr("New &Tab"),this,&TabWidget::createTab(,QKeySequence::AddTab));
    int index=tabBar()->tabAt(pos);//定位,index指代当前标签号
    if(index!=-1){
        QAction *action = menu.addAction(tr("Clone Tab"));//复制当前标签页
        connect(action, &QAction::triggered, this, [this,index]() {
            cloneTab(index);
        });
        action = menu.addAction(tr("Close &Other Tabs"));
        connect(action,&QAction::triggered,this,[this,index](){
            closeOtherTabs(index);
        });
        action = menu.addAction(tr("Reload Tab"));
        connect(action,&QAction::triggered,this,[this,index](){
            reloadTab(index);
        });
    }else{
        menu.addSeparator();
    }
    menu.addAction(tr("Reload All Tabs"),this,&TabWidget::reloadAllTabs);
    menu.exec(QCursor::pos());
}


/*
 * 创建一个webview
 * webView(currentIndex())：在第几个tab页创建一个webview
 * currentIndex():保存当前tab页索引
*/
WebView *TabWidget::currentWebView() const
{
    return webView(currentIndex());
}

/*
 * qobject_cast<WebView*>(widget(index));
 * 本方法返回object向下的转型WebView*，如果转型不成功则返回0，如果传入的object本身就是0则返回0。
 * 注：向上转型是安全的，向下转型是不安全的，需要开发人员保障其安全性。
*/
WebView *TabWidget::webView(int index) const
{
    return qobject_cast<WebView*>(widget(index));
}
