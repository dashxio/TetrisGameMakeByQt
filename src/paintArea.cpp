#include "paintArea.h"
#include "globalValue.h"
#include <QWidget>
#include <QPainter>
GameArea::GameArea(QWidget *parent) : QWidget(parent)
{
}

void GameArea::paintEvent(QPaintEvent *event)
{
    if(BlockItem::ItemsShadow != nullptr)
    {
        auto shadow = BlockItem::ItemsShadow;
        QPainter painter(this);
        QColor brush_color[4] = {QColor(Qt::white), QColor(Qt::red), QColor(Qt::blue), QColor(169, 169, 169)};
        for (int i = 0; i < LatticeRows; i++)
        {
            for (int j = 0; j < LatticeColumes; j++)
            {
                painter.setBrush(brush_color[shadow[i][j]]);
                if(shadow[i][j]==0)
                {
                    painter.setPen(Qt::NoPen);
                }
                else
                {
                    painter.setPen(Qt::SolidLine);
                }
                painter.drawRect(j*LatticeHeight, i*LatticeWide, LatticeWide, LatticeHeight);
            }
        }
    }
}

PromptArea::PromptArea(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(6 * LatticeWide, 6 * LatticeHeight);
}

void PromptArea::slotUpdateNextItem(QVector<QPoint> localxy)
{
    next_item_localxy = localxy;
    update();
}

void PromptArea::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setBrush(QColor(Qt::green));
    int painter_origin_dx = this->size().width() / 2 - LatticeWide;
    int painter_origin_dy = this->size().height() / 2 - 2*LatticeHeight;
    painter.save();
    painter.translate(painter_origin_dx, painter_origin_dy); //移动坐标系到widget中心
    for(auto &p: next_item_localxy)
    {
        painter.drawRect(p.x() * LatticeWide, p.y() * LatticeHeight, LatticeWide, LatticeHeight);
    }
    painter.restore();
}
