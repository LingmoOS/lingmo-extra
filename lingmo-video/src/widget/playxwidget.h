#ifndef PLAYXWIDGET_H
#define PLAYXWIDGET_H

#include <QWidget>
#include "eventpasswidget.h"

class PlayXWidget : public EventPassWidget
{
    Q_OBJECT
public:
    explicit PlayXWidget(QWidget *parent = nullptr);
    ~PlayXWidget();

signals:

};

#endif // PLAYXWIDGET_H
