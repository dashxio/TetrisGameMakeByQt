#include "gameWidget.h"
#include "globalValue.h"
#include <QLabel>
#include <QSpinBox>
#include <QGroupBox>
#include <QPushButton>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>


GameWidget::GameWidget(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("俄罗斯方块");

    BlockItem::initItemsShadow();
    m_item = new BlockItem(this);

    m_game_area = new GameArea(this);
    m_game_area->setMinimumSize(LatticeWide * LatticeColumes, LatticeHeight * LatticeRows);
    
    m_level_prompt = new QLabel(this);
    m_level_prompt->setText("难度：");
    //m_level_prompt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    m_level = new QSpinBox(this);
    m_level->setRange(1, 9);
    m_level->setValue(1);
    m_level->setPrefix("等级");
    //m_level->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    m_score_prompt = new QLabel(this);
    m_score_prompt->setText("得分：");
    //m_score_prompt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    m_score = new QLabel(this);
    m_score->setText(QString("%1").arg(0));
    //m_score->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    m_next_item_box = new QGroupBox(this);
    m_next_item_box->setTitle("下一个：");
    QHBoxLayout *tempLayout = new QHBoxLayout(m_next_item_box);
    m_next_area = new PromptArea(this);
    tempLayout->addWidget(m_next_area);
    m_next_item_box->setLayout(tempLayout);
    m_next_item_box->setMinimumSize(QSize(120,120));

    m_pause = new QPushButton(this);
    m_pause->setText("暂停");
    m_pause->hide();
    m_newgame = new QPushButton(this);
    m_newgame->setText("新游戏");

    connectSigAndSlots();

    QGridLayout *second_layout = new QGridLayout;
    //second_layout->setVerticalSpacing(5);
    second_layout->addWidget(m_level_prompt, 0, 0, 1, 1);
    second_layout->addWidget(m_level, 0, 1, 1, 1);
    second_layout->addWidget(m_score_prompt, 1, 0, 1, 1);
    second_layout->addWidget(m_score, 1, 1, 1, 1);
    second_layout->addWidget(m_next_item_box, 2, 0, 1, 2);
    second_layout->addWidget(m_pause, 3, 0, 1, 2);
    second_layout->addWidget(m_newgame, 4, 0, 1, 2);
    QGridLayout *fisrt_layout = new QGridLayout;
    fisrt_layout->addWidget(m_game_area, 0, 0, 2, 1);
    fisrt_layout->addLayout(second_layout, 0, 1, 1, 1);
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 378, QSizePolicy::Minimum, QSizePolicy::Expanding);
    fisrt_layout->addItem(verticalSpacer, 1, 1, 1, 1);
    this->setLayout(fisrt_layout);
    this->setFixedSize(sizeHint());
}

void GameWidget::clickedNewGame()
{
    if(m_newgame->text() == "新游戏")
    {
        m_pause->show();
        emit sigBeginGame();
        is_running = true;
        m_newgame->setText("停止游戏");
    }
    else if(m_newgame->text() == "停止游戏")
    {
        is_running = false;
        emit sigStopGame();
        m_pause->setText("暂停");
        m_pause->hide();
        m_newgame->setText("新游戏");
    }
}

void GameWidget::clickedPause()
{
    if(m_pause->text() == "暂停")
    {
        is_running = false;
        m_pause->setText("继续游戏");
        emit sigKeepRunning(false);
    }
    else if(m_pause->text() == "继续游戏")
    {
        is_running = true;
        emit sigKeepRunning(true);
        m_pause->setText("暂停");
    }
}

void GameWidget::connectSigAndSlots()
{
    //PushButton和this之间的通信
    connect(m_newgame, QPushButton::clicked, this, GameWidget::clickedNewGame);
    connect(m_pause, QPushButton::clicked, this, GameWidget::clickedPause);
    connect(m_level, QSpinBox::valueChanged, this, [=]{
                int move_gap = (11 - m_level->value()) * 100 > 200 ? (11 - m_level->value()) * 100 : 200;
                emit sigChangeMoveGap(move_gap);});

    //this和BlockItem之间的通信
    connect(this, GameWidget::sigBeginGame, m_item, BlockItem::slotBeginGame);
    connect(this, GameWidget::sigKeepRunning, m_item, BlockItem::slotKeepOrPasueMove);
    connect(this, GameWidget::sigStopGame, m_item, BlockItem::slotStopGame);
    connect(this, GameWidget::sigChangeMoveGap, m_item, BlockItem::changeMoveGap);
    connect(m_item, BlockItem::sigAddScore, this, GameWidget::addScore);
    connect(m_item, BlockItem::sigGameOver, this, [=]
            {   is_running = false; 
                m_pause->setText("暂停");
                m_pause->hide();
                m_newgame->setText("新游戏"); 
                QMessageBox::information(this, "", QString("游戏结束，得分: ") + m_score->text(), QMessageBox::Yes, QMessageBox::Yes);});
    
    //BlockItem和PaintArea(game_area和prompt_area)之间的通信
    connect(m_item, BlockItem::sigRenderGame, m_game_area, [=]()
            { m_game_area->update(); });
    connect(m_item, BlockItem::sigRenderNextItem, m_next_area, PromptArea::slotUpdateNextItem);
}

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    if(is_running)
    {
        switch (event->key())
        {
        case Qt::Key_Up:
            m_item->rotateSelf();
            break;
        case Qt::Key_Left:
            m_item->itemMoveOneStep(BlockItem::LEFT);
            break;
        case Qt::Key_Right:
            m_item->itemMoveOneStep(BlockItem::RIGHT);
            break;
        case Qt::Key_Down:
            m_item->itemMoveOneStep(BlockItem::DOWNWARD);
            break;
        case Qt::Key_Space:
            m_item->itemMovToBottom();
            break;
        default:
            break;
        }
    }
}

void GameWidget::addScore(int score)
{
    m_score_value += score;
    m_score->setText(QString("%1").arg(m_score_value));

    // 每获得200分，游戏难度增加一个等级
    if (m_level->value() < m_score_value / 200 + 1)
    {
        m_level->setValue(m_score_value / 200 + 1);  
        int move_gap = (11 - m_level->value()) * 100 > 200 ? (11 - m_level->value()) * 100 : 200;
        emit sigChangeMoveGap(move_gap);
    }
}
