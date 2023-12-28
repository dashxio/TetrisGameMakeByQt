#include "blockItem.h"
#include "globalValue.h"
#include <QTime>
#include <QtGlobal> // 引入QtGlobal头文件
#include <QTimerEvent>
#include <QMessageBox>

BlockItem::BlockItem(QObject *parent) : QObject(parent)
{
    //防止构建对象前忘记初始化item所在的环境
    if(ItemsShadow == nullptr)
    {
        initItemsShadow();
    }
    //initItem();
    m_move_gap = 1000; //打开主界面时的默认时间间隔是1000ms
}

void BlockItem::slotBeginGame()
{
    initItem();
    m_timerid = startTimer(m_move_gap); 
}

void BlockItem::slotKeepOrPasueMove(bool is_move)
{
    if(is_move)
    {
        m_timerid = startTimer(m_move_gap); 
    }
    else if(m_timerid != 0)
    {
        killTimer(m_timerid);
        m_timerid = 0;
    }   
}

void BlockItem::slotStopGame()
{
    slotKeepOrPasueMove(false);
    initItemsShadow();
}

void BlockItem::initItem()
{
    //初始化当前以及下一个item的坐标。
    m_location.setX(ItemInitPosition);
    m_location.setY(0);
    if(m_next_shape == NOSHAPE)
    {
        srand(QTime::currentTime().msec());
        m_current_shape = static_cast<ItemShape>(rand() % static_cast<int>(BlockItem::NOSHAPE));
        m_current_orient = rand() % 4;
    }
    else
    {
        m_current_shape = m_next_shape;
        m_current_orient = m_next_orient;
    }
    srand(QTime::currentTime().msec());
    m_next_shape = static_cast<ItemShape>(rand() % static_cast<int>(BlockItem::NOSHAPE));
    m_next_orient = rand() % 4;
    
    QPoint current_points[4], next_points[4];
    QVector<QPoint> emit_points;
    getLocalPoints(current_points, m_current_shape, m_current_orient);
    getLocalPoints(next_points, m_next_shape, m_next_orient);
    for (int i = 0; i < 4; i++)
    {
        emit_points.append(next_points[i]);
    }
    updateItemShadow(current_points, current_points);
    emit sigRenderNextItem(emit_points);
    emit sigRenderGame();
}


void BlockItem::rotateSelf(RotateDirection rdir)
{
    int temp_orient;
    switch (rdir)
    {
    case CLOCKWISE:
        temp_orient = (m_current_orient + 1) % 4;
        break;
    case ANTICLOCKWISE:
        temp_orient = m_current_orient - 1 < 0 ? m_current_orient + 3 : m_current_orient - 1;
        break;
    }
    QPoint current_points[4];
    QPoint temp_points[4];
    getLocalPoints(current_points);
    getLocalPoints(temp_points, m_current_shape, temp_orient);
    int is_rotateable = checkPlaceable(m_location, temp_points);
    if (is_rotateable == 0) // 可以旋转
    {
        updateItemShadow(current_points, temp_points);
        m_current_orient = temp_orient;
        emit sigRenderGame();
    }
}

void BlockItem::itemMoveOneStep(MoveDirection mdir)
{
    QPoint move_dxy{0,0};
    switch (mdir)
    {
    case DOWNWARD:
        move_dxy.ry()++;
        break;
    case LEFT:
        move_dxy.rx()--;
        break;
    case RIGHT:
        move_dxy.rx()++;
    default:
        break;
    }
    QPoint current_points[4];
    getLocalPoints(current_points);
    int is_moveable = checkPlaceable(m_location + move_dxy, current_points);
    if (is_moveable == 0) // 可以移动
    {
        updateItemShadow(current_points, current_points, move_dxy);
        m_location += move_dxy;
        emit sigRenderGame();
    }
    else if(mdir == DOWNWARD) //移动到底了
    {
        addSelfToShadow();
        for (int j = 1; j < LatticeColumes-1; j++)
        {
            if(ItemsShadow[1][j]==2)
            {
                slotStopGame();
                emit sigGameOver();
            }
        }

        auto maxY = [&current_points]
        {
            int y = 0;
            for (auto &p : current_points)
            {
                if (p.y() > y)
                    y = p.y();
            }
            return y;
        };

        int min_row = m_location.y();
        int max_row = m_location.y() + maxY();
        eliminateRowsBlocks(min_row, max_row); // 消除整行并且发送得分更新的信号
        initItem(); //下一位选手入场。
    }

}

void BlockItem::itemMovToBottom()
{
    QPoint move_onestep{0, 1}; //向下移动
    QPoint current_localxy[4];
    getLocalPoints(current_localxy);

    //向下移动到“不能再向下移动的”位置。
    while(checkPlaceable(m_location + move_onestep, current_localxy) == 0)
    {
        updateItemShadow(current_localxy, current_localxy, move_onestep);
        m_location += move_onestep;
        emit sigRenderGame();
    }
}

void BlockItem::addSelfToShadow()
{
    QPoint current_localxy[4];
    getLocalPoints(current_localxy);
    for (int i = 0; i < 4; i++)
    {
        int last_time_block_ix = m_location.x() + current_localxy[i].x();
        int last_time_block_iy = m_location.y() + current_localxy[i].y();
        ItemsShadow[last_time_block_iy][last_time_block_ix] = 2;
    }
}

void BlockItem::changeMoveGap(int microsecond)
{
    m_move_gap = microsecond;
    if(m_timerid != 0)
    {
        killTimer(m_timerid);
        m_timerid = startTimer(m_move_gap);
    }    
}


void BlockItem::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_timerid)
    {
        itemMoveOneStep(DOWNWARD);
    }
}


void BlockItem::initItemsShadow()
{
    if(ItemsShadow == nullptr)
    {
        ItemsShadow = new int *[LatticeRows];
        for (int i = 0; i < LatticeRows; i++)
        {
            ItemsShadow[i] = new int[LatticeColumes];
        }
    }

    for (int i = 0; i < LatticeRows; i++)
    {
        for (int j = 0; j < LatticeColumes; j++)
        {
            if( i==LatticeRows-1 || j==0 || j==LatticeColumes-1 )
            {
                ItemsShadow[i][j] = 3; //3代表墙壁，2代表尸体，1代表自己。
            }
            else
            {
                ItemsShadow[i][j] = 0; //0代表空
            }
        }
    }

}

void BlockItem::updateItemShadow(QPoint *last_localxy, QPoint *current_localxy, QPoint move_dxdy)
{
    for (int i = 0; i < 4; i++)
    {
        int last_time_block_ix = m_location.x() + last_localxy[i].x();
        int last_time_block_iy = m_location.y() + last_localxy[i].y();
        ItemsShadow[last_time_block_iy][last_time_block_ix] = 0;
    }

    QPoint item_location = m_location + move_dxdy;
    for (int i = 0; i < 4; i++)
    {
        int current_time_block_ix = item_location.x() + current_localxy[i].x();
        int current_time_block_iy = item_location.y() + current_localxy[i].y();
        ItemsShadow[current_time_block_iy][current_time_block_ix] = 1;
    }    
}

int BlockItem::checkPlaceable(QPoint item_location, QPoint *item_localxy)
{
    int return_value = 0;
    for (int i = 0; i < 4; i++)
    {
        int item_x = item_location.x() + item_localxy[i].x();
        int item_y = item_location.y() + item_localxy[i].y();
        int shadow_value = ItemsShadow[item_y][item_x];
        if(shadow_value==0 || shadow_value==1)
        {
            continue;
        }        
        if(shadow_value==2)
        {
            return 2;
        }
        if(shadow_value==3)
        {
            return_value = 3;
        }                        
    }
    return return_value;
}

void BlockItem::getLocalPoints(QPoint *localxy, ItemShape shape, int orient)
{
    for (int i = 0; i < 4; i++)
    {
        localxy[i] = BaseItemTable[shape][orient][i];
    }
}

void BlockItem::getLocalPoints(QPoint *localxy)
{
    getLocalPoints(localxy, m_current_shape, m_current_orient);
}

void BlockItem::eliminateRowsBlocks(int min_row, int max_row)
{
    //lambda表达式，消除行m，m行上面的值下降一格
    auto remove_this_row = [=](int m)
    {
        for (int i = m; i >= 0; i--)
        {
            if(i >= 1)
            {
                for (int j = 1; j < LatticeColumes - 1; j++)
                {
                    ItemsShadow[i][j] = ItemsShadow[i - 1][j];
                }
            }
            else
            {
                for (int j = 1; j < LatticeColumes - 1; j++)
                {
                    ItemsShadow[i][j] = 0;
                }
            }
        }
    };

    //
    int deleted_rows = 0;
    for (int i = min_row; i < max_row + 1; i++)
    {
        int shadows_one_row = 0;
        for (int j = 1; j < LatticeColumes - 1; j++)
        {
            if(ItemsShadow[i][j] == 2)
            {
                shadows_one_row++;
            }
        }
        if (shadows_one_row == LatticeColumes - 2)
        {
            remove_this_row(i);
            deleted_rows++;
        }
    }

    if(deleted_rows == 0){
        return;
    }

    //得分使用y = 20(x-1)^2+10的函数关系，鼓励一次消多行
    int add_score = 20 * (deleted_rows - 1) * (deleted_rows - 1) + 10;
    emit sigAddScore(add_score);
}
