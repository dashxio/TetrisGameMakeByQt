#ifndef PAINTAREA_H
#define PAINTAREA_H

#include <QWidget>
#include "blockItem.h"

//主界面游戏区
class GameArea : public QWidget
{
    Q_OBJECT
public:

    GameArea(QWidget *parent=nullptr);

private:
    void paintEvent(QPaintEvent *event) override;

};

//主界面提示区
class PromptArea : public QWidget
{
    Q_OBJECT
public:
    PromptArea(QWidget *parent=nullptr);
    void slotUpdateNextItem(QVector<QPoint>);

private:
    //在提示区绘制下一个Item
    void paintEvent(QPaintEvent* event);

private:
    QVector<QPoint> next_item_localxy;
};

#endif
