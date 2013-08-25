#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <KXmlGuiWindow>

class QDeclarativeView;
class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0);
    virtual ~MainWindow();
    virtual void closeEvent(QCloseEvent* e);
private:
    QDeclarativeView* m_declarativeView;
};

#endif
