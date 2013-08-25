#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <KMainWindow>

class QDeclarativeView;
class MainWindow : public KMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0);
    virtual ~MainWindow();
    virtual void closeEvent(QCloseEvent* e);
private:
    QDeclarativeView* m_declarativeView;
public slots:
    void updateInfo();
};

#endif
