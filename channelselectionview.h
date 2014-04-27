#ifndef CHANNELSELECTIONVIEW_H
#define CHANNELSELECTIONVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QLabel>
#include <QIcon>
#include <QDebug>

class ChannelSelectionView : public QWidget
{
    Q_OBJECT
public:
    explicit ChannelSelectionView(QWidget *parent = 0);

    void paintEvent(QPaintEvent *event);
    QSize sizeHint();

    int x, y, w, h;

    QPushButton *up1;
    QPushButton *down1;
    QPushButton *up2;
    QPushButton *down2;

    QGraphicsView *numView1;
    QGraphicsView *numView2;

    QGraphicsScene *numScene1;
    QGraphicsScene *numScene2;

    QLabel *primaryLabel;
    QLabel *secondaryLabel;

    QGraphicsTextItem *primaryNum;
    QGraphicsTextItem *secondaryNum;

    int channel1;
    int channel2;

    
signals:
    void up1Pressed();
    void up2Pressed();

    void down1Pressed();
    void down2Pressed();
    
public slots:
    void setXYWH(int x, int y, int w, int h);
    void up1Clicked();
    void up2Clicked();
    void down1Clicked();
    void down2Clicked();

    void setPrimaryChan(int channel);
    void setSecondaryChan(int channel);
    
};

#endif // CHANNELSELECTIONVIEW_H
