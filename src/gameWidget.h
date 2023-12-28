#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include "paintArea.h"
#include "blockItem.h"
class QLabel;
class QSpinBox;
class QGroupBox;
class QPushButton;

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    GameWidget(QWidget *parent=nullptr);

    void clickedNewGame();
    void clickedPause();

    //增加得分，当接受到Item的增加得分的信号时触发。
    void addScore(int score);

signals:
    void sigBeginGame();
    void sigKeepRunning(bool is_run);
    void sigStopGame();

    //通知Item改变每步移动的时间间隔，用于level发生改变时。
    void sigChangeMoveGap(int microsecond);

private:
    void connectSigAndSlots();
    void keyPressEvent(QKeyEvent *event) override;

private:
    BlockItem *m_item;
    GameArea *m_game_area;
    QLabel *m_level_prompt;
    QSpinBox *m_level;
    QLabel *m_score_prompt, *m_score;
    QGroupBox *m_next_item_box;
    PromptArea *m_next_area;
    QPushButton *m_pause, *m_newgame;
    bool is_running = false;
    int m_score_value = 0;
};

#endif
