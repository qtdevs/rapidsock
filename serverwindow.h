#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QWidget>
#include "rapidserver.h"

namespace Ui {
class ServerWindow;
}

class ServerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = 0);
    virtual ~ServerWindow();
private:
    QScopedPointer<Ui::ServerWindow> ui;

private Q_SLOTS:
    void on_writeButton_clicked();
    void on_stateButton_clicked();
    void on_buildButton_clicked();
private:
    RapidServer *server;

private:
    QHash<RapidClient *, int> socketIds;
};

#endif // SERVERWINDOW_H
