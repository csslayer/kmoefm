#include "mainwindow.h"
#include "app.h"
#include "themeiconprovider.h"
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <kdeclarative.h>
#include <KStandardAction>
#include <KActionCollection>
#include <KMenuBar>

MainWindow::MainWindow(QWidget* parent): KXmlGuiWindow(parent)
    ,m_declarativeView(new QDeclarativeView(this))
{
    KStandardAction::quit(moeApp, SLOT(quit()), actionCollection());
    setupGUI( Keys  | Create);
    menuBar()->hide();
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
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    QWidget::closeEvent(e);
    moeApp->quit();
}

