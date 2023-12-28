#ifndef BLOCKITEM_H
#define BLOCKITEM_H

#include <QVector>
#include <QPoint>
#include <QObject>

constexpr int ItemShapeNum = 7;
constexpr int ItemOrientNum = 4;
constexpr int BlocksOfBaseItem = 4;

//这是一个包含若干格子的类
class BlockItem : public QObject
{
    Q_OBJECT
public:
    //方块的形状
    enum ItemShape
    {
        YIZI = 0,      // “一”字形
        TIANZI = 1,    // “田”字形
        SHANZI = 2,    // “山”字形
        QIZI = 3,      // “7”字形
        FANQIZI = 4,   // 反“7”字形
        ZZI = 5,       // “z”字形
        FANZZI = 6,    // 反“z”字形
        NOSHAPE = 7 // 其他形状
    };
    //旋转的方向
    enum RotateDirection
    {
        CLOCKWISE = 0, //顺时针
        ANTICLOCKWISE = 1 //逆时针
    };
    //移动的方向
    enum MoveDirection
    {
        DOWNWARD = 0,
        LEFT = 1,
        RIGHT = 2
    };

    BlockItem(QObject *parent = nullptr);

    //开始游戏
    void slotBeginGame();

    //item保持或暂停移动
    void slotKeepOrPasueMove(bool is_move);

    //停止移动，GameOver时也会触发。
    void slotStopGame();

    //旋转自身
    void rotateSelf(RotateDirection rdir = CLOCKWISE); 

    //移动一格，用于timeEvent()中，每隔一定时间向下移动一格；和左右下键被触发时会移动一格
    void itemMoveOneStep(MoveDirection mdir);

    //移动到底，用于空格键被点击时
    void itemMovToBottom();

    //在ItemsShadow中留下自己的位置，在Item移动到最底端时调用
    void addSelfToShadow();

    //改变每次向下移动的时间间隔，用于游戏等级改变时。
    void changeMoveGap(int microsecond);

signals:
    //在Item移动到最底端时，如果消除整行了会emit，用于告知主界面GameWidget增加分数。
    void sigAddScore(int Score);

    //用于告知GameArea渲染游戏画面
    void sigRenderGame();
    
    //用于告知GameWidget中的PromptArea渲染下一个Item的提示信息。
    void sigRenderNextItem(QVector<QPoint> points);

    //用于告知主界面游戏结束了
    void sigGameOver();

protected:
    void timerEvent(QTimerEvent *event);

public:
    //不同形状不同取向的基本方块所含的小方块的局部坐标的表，不同行代表不同类型的Item，不同列代表不同取向的Item。
    static constexpr QPoint BaseItemTable[ItemShapeNum][ItemOrientNum][BlocksOfBaseItem] = {
        {{{0, 1}, {1, 1}, {2, 1}, {3, 1}}, {{1, 0}, {1, 1}, {1, 2}, {1, 3}}, {{0, 1}, {1, 1}, {2, 1}, {3, 1}}, {{1, 0}, {1, 1}, {1, 2}, {1, 3}}},   // “一”字形      
        {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}},   // “田”字形
        {{{0, 1}, {1, 1}, {1, 0}, {2, 1}}, {{1, 0}, {1, 1}, {2, 1}, {1, 2}}, {{0, 1}, {1, 1}, {1, 2}, {2, 1}}, {{1, 0}, {1, 1}, {1, 2}, {0, 1}}},   // “山”字形
        {{{0, 0}, {1, 0}, {1, 1}, {1, 2}}, {{2, 0}, {2, 1}, {1, 1}, {0, 1}}, {{2, 2}, {1, 2}, {1, 1}, {1, 0}}, {{0, 2}, {0, 1}, {1, 1}, {2, 1}}},   // “7”字形
        {{{2, 0}, {1, 0}, {1, 1}, {1, 2}}, {{2, 2}, {2, 1}, {1, 1}, {0, 1}}, {{1, 0}, {0, 2}, {1, 2}, {1, 1}}, {{0, 0}, {0, 1}, {1, 1}, {2, 1}}},   // 反“7”字形
        {{{0, 0}, {1, 0}, {1, 1}, {2, 1}}, {{2, 0}, {2, 1}, {1, 1}, {1, 2}}, {{0, 0}, {1, 0}, {1, 1}, {2, 1}}, {{2, 0}, {2, 1}, {1, 1}, {1, 2}}},   // “z”字形
        {{{2, 0}, {1, 0}, {1, 1}, {0, 1}}, {{2, 2}, {2, 1}, {1, 1}, {1, 0}}, {{2, 0}, {1, 0}, {1, 1}, {0, 1}}, {{2, 2}, {2, 1}, {1, 1}, {1, 0}}}    // 反“z”字形       
    };

    //用一个静态数组表示Item的影子（或者说所处的环境），用于GameView的绘制和检测接触（碰撞），就像人通过光线感知世界一样，Item通过ItemsShadow获取环境信息，知晓自己是否撞墙或撞到尸体上了。
    static inline int **ItemsShadow = nullptr;

    //初始化Item所在的环境，一般应在实例化Item对象之前调用，也就是先初始化环境，再构建item。
    static void initItemsShadow();
    
private:

    //随机初始化当前以及下一个item的形状和取向。
    void initItem();

    //把item的影子加入到环境中，应在构建item和item移动过程中全程调用。
    void updateItemShadow(QPoint *last_localxy, QPoint *current_localxy, QPoint move_dxdy = QPoint(0, 0));

    //检查当前的环境是否可以放置item，返回0代表可以，返回2代表撞到了堆积的items的尸体，返回3代表撞墙了
    int checkPlaceable(QPoint item_location, QPoint* item_localxy);

    //获取某种shape和orient的Item中的小格子相对于Item位置点的相对坐标
    void getLocalPoints(QPoint* localxy, ItemShape shape, int orient);
    void getLocalPoints(QPoint *localxy); //后两个参数不填则默认为当前形状和取向。

    //检测从min_row到max_row的整行并消除，发出分数增加的信号。
    void eliminateRowsBlocks(int min_row, int max_row);

private:
    QPoint m_location; //当前Item本身所在的位置
    int m_timerid = 0;
    int m_move_gap;
    ItemShape m_current_shape = NOSHAPE, m_next_shape = NOSHAPE; //Item的形状
    int m_current_orient, m_next_orient; //Item的朝向:0,1,2,3
};


#endif
