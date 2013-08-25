#include "mainwindow.h"
#include "app.h"
#include "themeiconprovider.h"
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <kdeclarative.h>

MainWindow::MainWindow(QWidget* parent): KMainWindow(parent)
    ,m_declarativeView(new QDeclarativeView(this))
{
    KDeclarative kdeclarative;
    //view refers to the QDeclarativeView
    kdeclarative.setDeclarativeEngine(m_declarativeView->engine());
    kdeclarative.initialize();
    //binds things like kconfig and icons
    kdeclarative.setupBindings();
    setCentralWidget(m_declarativeView);
    m_declarativeView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    m_declarativeView->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    m_declarativeView->engine()->addImageProvider("theme", new ThemeIconProvider);
    m_declarativeView->rootContext()->setContextProperty("controller", moeApp->controller());
    m_declarativeView->setSource(QUrl("qrc:/main.qml"));
    setMinimumHeight(192);
    setMinimumWidth(192 * 2);

    connect(moeApp->controller(), SIGNAL(infoChanged()), SLOT(updateInfo()));
    updateInfo();
}

MainWindow::~MainWindow()
{
}

void MainWindow::updateInfo()
{
    QString title = (*moeApp->controller()->info())["title"].toString();
    if (title.isEmpty()) {
        title = QLatin1String("MoeFM");
    } else {
        title = QString("%1 - MoeFM").arg(title);
    }
    setWindowTitle(title);
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    QWidget::closeEvent(e);
    moeApp->quit();
}

