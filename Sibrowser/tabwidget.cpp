
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
        emit webActionEnabledChanged(QWebEnginePage::Back,view->isWebActionEnabled(QWebEnginePage::Back));
        emit webActionEnabledChanged(QWebEnginePage::Forward,view->isWebActionEnabled(QWebEnginePage::Forward));
        emit webActionEnabledChanged(QWebEnginePage::Stop,view->isWebActionEnabled(QWebEnginePage::Stop));
        emit webActionEnabledChanged(QWebEnginePage::Reload,view->isWebActionEnabled(QWebEnginePage::Reload));
    }else{
        emit titleChanged(QString());
        emit loadProgress(0);
        emit urlChanged(QUrl());
        emit iconChanged(QIcon(QStringLiteral(":defaulticon.png")));
        emit webActionEnabledChanged(QWebEnginePage::Back,false);
        emit webActionEnabledChanged(QWebEnginePage::Forward,false);
        emit webActionEnabledChanged(QWebEnginePage::Stop,false);
        emit webActionEnabledChanged(QWebEnginePage::Reload,false);
    }
}

//右击tab标签会出现一个菜单
void TabWidget::handleContextMenuRequested(const QPoint &pos)
{
    QMenu menu;
    menu.addAction(tr("New &Tab"),this,&TabWidget::createTab,QKeySequence::AddTab);
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

/*
 * webView->page():该页面的地址
 * indexOf()方法从字符串的开头向后搜索字符串，然后返回子字符串的位置（如果没有子字符串的位置，则返回-1）
 * lastIndexOf()方法是从字符串的末尾向前搜索子字符串。
 *
 * linkHovered:鼠标移动到连接，底部展示网址
 * loadStarted：信号当有新的web page请求时发出
 * loadProgress信号在load web page的一个元素成功时发出。
 * 这里的元素指代image、text、script对象。loadProgress的value表明了网页load的进度，范围从0-100。
 * 我们可以看出，QWebView封装了获得load进度值的计算方法。我们可以连接信号，对进度条赋值。
 * loadFinished：信号在web page load完成后发送
 *
 *
*/
void TabWidget::setupView(WebView *webView)
{
    QWebEnginePage *webPage = webView->page();
    connect(webView, &QWebEngineView::titleChanged, [this, webView](const QString &title) {
        int index = indexOf(webView);
        if (index != -1)
            setTabText(index, title);
        if (currentIndex() == index)
            emit titleChanged(title);
    });
    connect(webView, &QWebEngineView::urlChanged, [this, webView](const QUrl &url) {
        int index = indexOf(webView);
        if (index != -1)
            tabBar()->setTabData(index, url);
        if (currentIndex() == index)
            emit urlChanged(url);
    });
    connect(webView, &QWebEngineView::loadProgress, [this, webView](int progress) {
        if (currentIndex() == indexOf(webView))
            emit loadProgress(progress);
    });
    connect(webPage, &QWebEnginePage::linkHovered, [this, webView](const QString &url) {
        if (currentIndex() == indexOf(webView))
            emit linkeHovered(url);
    });
    connect(webPage, &WebPage::iconChanged, [this, webView](const QIcon &icon) {
        int index = indexOf(webView);
        QIcon ico = icon.isNull() ? QIcon(QStringLiteral(":defaulticon.png")) : icon;

        if (index != -1)
            setTabIcon(index, ico);
        if (currentIndex() == index)
            emit iconChanged(ico);
    });
    connect(webView, &WebView::webActionEnableChanged, [this, webView](QWebEnginePage::WebAction action, bool enabled) {
        if (currentIndex() ==  indexOf(webView))
            emit webActionEnabledChanged(action,enabled);
    });
    connect(webView, &QWebEngineView::loadStarted, [this, webView]() {
        int index = indexOf(webView);
        if (index != -1) {
            QIcon icon(QLatin1String(":view-refresh.png"));
            setTabIcon(index, icon);
        }
    });
    connect(webPage, &QWebEnginePage::windowCloseRequested, [this, webView]() {
        int index = indexOf(webView);
        if (index >= 0)
            closeTab(index);
    });
}

/*
 * 创建一个tab标签页
 * QWebEngineProfile::defaultProfile()：设置一个默认的缓存，将webview放入其中
 * 创建新的标签页
 * setCurrentWidget(webView)：将webview放入当前的widget中
*/
WebView *TabWidget::createTab(bool makeCurrent)
{
    WebView *webView = new WebView;
    WebPage *webPage = new WebPage(QWebEngineProfile::defaultProfile(),webView);
    webView->setPage(webPage);
    setupView(webView);
    addTab(webView,tr("new tab page"));
    if(makeCurrent)
        setCurrentWidget(webView);//使当前界面为新创建的界面
    return webView;
}

//reload current tab
void TabWidget::reloadAllTabs()
{
    for(int i=0;i<count();i++)
        webView(i)->reload();
}

//close other tabs
void TabWidget::closeOtherTabs(int index)
{
    for(int i=count()-1;i>=0;i--){
        if(i!=index)
            closeTab(i);
    }
}

//关闭将当前标签页
/*
 * 将需要关闭的webview的地址赋值给view
 * 查看该webview是否获取焦点
 * 先移除tab标签
 * 判断，如果需要移除的webview是选中状态，并且当前的tab个数大于0，那么，除这个webview之外最新建的webview获取焦点，就是展示出来。
 * 判断，如果tab个数等于0，那么创建一个新的tab。
 * view删除对应的Later
*/
void TabWidget::closeTab(int index)
{
    if (WebView *view = webView(index)){
        bool hasFocus = view->hasFocus();
        removeTab(index);
        if(hasFocus && count()>0)
            currentWebView()->setFocus();
        if(count()==0)
            createTab();
        view->deleteLater();
    }
}

//clone current tab
void TabWidget::cloneTab(int index)
{
    if(WebView *view = webView(index)){
        WebView *tab = createTab(false);
        tab->setUrl(view->url());
    }
}

//set url
void TabWidget::setUrl(const QUrl &url)
{
    if(WebView *view=currentWebView()){
        view->setUrl(url);//直接获取当前页面的url
        view->setFocus();//定位至当前页面
    }
}

/*
 * 触发webpage事件，并且使该页面获取焦点
 * 该函数在其他.cpp文件中调用。
*/
void TabWidget::triggerWebPageAction(QWebEnginePage::WebAction action)
{
    if(WebView *webView = currentWebView()){
        webView->triggerPageAction(action);
        webView->setFocus();
    }
}

//get next tab
void TabWidget::nextTab()
{
    int next = currentIndex()-1;
    if(next==count())
        next=0;
    setCurrentIndex(next);

}

//get previous tab
void TabWidget::previousTab()
{
    int pre = currentIndex()-1;
    if(pre<0)
        pre=count()-1;
    setCurrentIndex(pre);
}

//reload tab page
void TabWidget::reloadTab(int index)
{
    if(WebView *view = webView(index))
        view->reload();
}
