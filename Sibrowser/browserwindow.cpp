#include "browser.h"
#include "browserwindow.h"
#include "tabwidget.h"
#include "urllineedit.h"
#include "webview.h"
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>

//initialize list
BrowserWindow::BrowserWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_tabWidget(new TabWidget(this))
    , m_progressBar(new QProgressBar(this))
    , m_historyBackAction(nullptr)
    , m_historyForwardAction(nullptr)
    , m_stopAction(nullptr)
    , m_reloadAction(nullptr)
    , m_stopReloadAction(nullptr)
    , m_urlLineEdit(new UrlLineEdit(this))
{
    /*
     * 设置按钮风格，只显示一个图标，文本或文本位于图标旁边、下方。默认值是Qt::ToolButtonIconOnly。
     * Qt::ToolButtonFollowStyle：遵循QStyle::StyleHint
    */
    setToolButtonStyle(Qt::ToolButtonFollowStyle);
    /*
     * 该方法的意思是当退出窗口时候，Qt会delete，销毁窗口，所以实例化窗口时，
     * 在栈上实例化，使用了该方法，退出窗口时会报错，
     * 而在堆上实例化窗口，则不会报错，因为堆上需要我们手动delete释放内存
    */
    setAttribute(Qt::WA_DeleteOnClose, true);

    /*
     * 创建一个ToolBar,在这里就是搜索框所在的那一条，可以拖动的控件
    */
    QToolBar *toolbar = createToolBar();
    addToolBar(toolbar);

    //添加菜单栏
    menuBar()->addMenu(createFileMenu(m_tabWidget));
    menuBar()->addMenu(createViewMenu(toolbar));

    //实例化一个Widget
    QWidget *centralWidget = new QWidget();

    //进行页面布局,布局方式为垂直布局
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);//表示各个控件的上下间距
    layout->setMargin(0);//表示控件与窗体的左右间距
    addToolBarBreak();
    /*
    * setMaximumHeight(1)：设置进度条，高度为1
    * setTextVisible(false)：设置进度条，百分之几这个数字不显示，true就是显示
    * setStyleSheet():设置进度条样式
    */
    m_progressBar->setMaximumHeight(1);
    m_progressBar->setTextVisible(true);
    m_progressBar->setStyleSheet(QStringLiteral("QProgressBar {border: 0px } QProgressBar::chunk { background-color: blue; }"));
    layout->addWidget(m_progressBar);

    //添加一个Widget,放置tab
    layout->addWidget(m_tabWidget);

    centralWidget->setLayout(layout);

    //主窗口类的setCentralWidget()方法,它将把widget设置为主窗口的中心窗口部件
    setCentralWidget(centralWidget);

    //------一系列的信号槽函数------
    //tab页改变，窗口的title改变
    connect(m_tabWidget, &TabWidget::titleChanged, this, &BrowserWindow::handleWebViewTitleChanged);
    //鼠标移动到连接上面statusBar部分会显示相应连接内容
    connect(m_tabWidget, &TabWidget::linkeHovered, [this](const QString& url) {
        statusBar()->showMessage(url);
    });
    //加载页面的时候，进度条进行读取
    connect(m_tabWidget, &TabWidget::loadProgress, this, &BrowserWindow::handleWebViewLoadProgress);
    //url改变的时候，输入框的URL就要进行修改
    connect(m_tabWidget, &TabWidget::urlChanged, this, &BrowserWindow::handleWebViewUrlChanged);
    //icon图改变的时候，输入框的ico图就要进行修改
    connect(m_tabWidget, &TabWidget::iconChanged, this, &BrowserWindow::handleWebViewIconChanged);
    //
    connect(m_tabWidget, &TabWidget::webActionEnabledChanged, this, &BrowserWindow::handleWebActionEnabledChanged);
    //输入框输入地址，按回车进入相应页面，并且在输入框显示默认图标
    connect(m_urlLineEdit, &QLineEdit::returnPressed, this, [this] {
        m_urlLineEdit->setFavIcon(QIcon(QStringLiteral(":defaulticon.png")));
        loadPage(m_urlLineEdit->url());
    });

    //刚打开浏览器加载页面的时候，url输入框显示默认icon图
    m_urlLineEdit->setFavIcon(QIcon(QStringLiteral(":defaulti()con.png")));
    handleWebViewTitleChanged(tr("Qt browser"));
    m_tabWidget->createTab();


}

BrowserWindow::~BrowserWindow()
{
}

//重载sizehint()方法
QSize BrowserWindow::sizeHint() const
{
    QRect desktop = QApplication::desktop()->screenGeometry();
    QSize size = desktop.size()*qreal(0.9);
    return size;
}

//菜单栏第一个菜单
QMenu *BrowserWindow::createFileMenu(TabWidget *tabWidget)
{
    //set name of file bar
    QMenu *fileMenu = new QMenu(tr("&File"));
    //add new window
    fileMenu->addAction(tr("&Add new Window"),this,&BrowserWindow::handleNewWindowTriggered, QKeySequence::New);
    /*
    * 设置子菜单 new tab
    * new tab的图片，显示文字
    * 快捷键(添加setShortcuts()设置快捷键之后，在菜单后面就会显示相应的快捷键)
    * icon图不显示
    * 信号槽函数触发，connect(new tab子菜单，触发，tabwidget,创建一个tab页)
    * 将newTabAction添加到菜单列表
    */
    QAction *newTabAction = new QAction(QIcon(QLatin1String(":addtab.png")), tr("New &Tab"), this);
    newTabAction->setShortcut(QKeySequence::AddTab);
    newTabAction->setIconVisibleInMenu(false);
    connect(newTabAction, &QAction::triggered, tabWidget, &TabWidget::createTab);
    fileMenu->addAction(newTabAction);

    //add "Open File"
    fileMenu->addAction(tr("&Open File..."),this,&BrowserWindow::handleFileOpenTriggered, QKeySequence::Open);

    //添加不同tab之间的分割线
    fileMenu->addSeparator();

    //添加子菜单关闭当前tab
    QAction *closeTabAction = new QAction(QIcon(QLatin1String(":closetab.png")), tr("&Close Tab"), this);
    closeTabAction->setShortcut(QKeySequence::Close);
    closeTabAction->setIconVisibleInMenu(false);
    connect(closeTabAction, &QAction::triggered, [tabWidget]() {
        tabWidget->closeTab(tabWidget->currentIndex());
    });
    fileMenu->addAction(closeTabAction);

    //添加子菜单退出
    QAction *closeAction = new QAction(tr("&Quit"),this);
    closeTabAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
    connect(closeTabAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(closeAction);

    connect(fileMenu, &QMenu::aboutToShow, [closeAction]() {
        if (Browser::instance().windows().count() == 1)
            closeAction->setText(tr("&Quit"));
        else
            closeAction->setText(tr("&Close Window"));
    });
    return fileMenu;
}

QMenu *BrowserWindow::createViewMenu(QToolBar *toolBar)
{
    QMenu *viewMenu = new QMenu(tr("&View"));
}
