#include "channelselectionview.h"

ChannelSelectionView::ChannelSelectionView(QWidget *parent) :
    QWidget(parent)
{
    this->setVisible(false);

    channel1 = 0;
    channel2 = 1;

    up1 = new QPushButton(QIcon("C:/Users/Rene/Pictures/UpArrow.png"),"",this);
    up1->move(40,35);

    up2 = new QPushButton(QIcon("C:/Users/Rene/Pictures/UpArrow.png"),"",this);
    up2->move(130,35);

    down1 = new QPushButton(QIcon("C:/Users/Rene/Pictures/DownArrow.png"),"",this);
    down1->move(40,120);

    down2 = new QPushButton(QIcon("C:/Users/Rene/Pictures/DownArrow.png"),"",this);
    down2->move(130,120);

    connect(up1,SIGNAL(clicked()),this,SLOT(up1Clicked()));
    connect(up2,SIGNAL(clicked()),this,SLOT(up2Clicked()));

    connect(down1,SIGNAL(clicked()), this, SLOT(down1Clicked()));
    connect(down2,SIGNAL(clicked()),this,SLOT(down2Clicked()));


    numView1 = new QGraphicsView(this);
    numView2 = new QGraphicsView(this);

    QRect rect1,rect2;

    rect1.setRect(40,75,30,30);
    rect2.setRect(130,75,30,30);

    numView1->setGeometry(rect1);
    numView2->setGeometry(rect2);

    numScene1 = new QGraphicsScene(numView1);
    numScene2 = new QGraphicsScene(numView2);

    numView1->setScene(numScene1);
    numView2->setScene(numScene2);

    numScene1->addText("&00");
    numScene2->addText("&01");


    primaryLabel = new QLabel("Primary",this);
    secondaryLabel = new QLabel("Secondary",this);

    primaryLabel->move(30,150);
    secondaryLabel->move(130,150);


}

void ChannelSelectionView::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);

    if(channel1 <= 9){
        QString temp = "0"+QString::number(channel1,10);
        numScene1->clear();
        numScene1->addText(temp);
    }else{
        numScene1->clear();
        numScene1->addText(QString::number(channel1,10));
    }

    if(channel2 <= 9){
        QString temp = "0"+QString::number(channel2,10);
        numScene2->clear();
        numScene2->addText(temp);
    }else{
        numScene2->clear();
        numScene2->addText(QString::number(channel2,10));
    }

    this->setWindowOpacity(0.75);
    painter.drawRect(this->geometry());

    painter.end();

}

QSize ChannelSelectionView::sizeHint()
{
    return QSize(w,h);
}


void ChannelSelectionView::setXYWH(int x, int y, int w, int h)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;

    this->setGeometry(x,y,w,h);
}

void ChannelSelectionView::up1Clicked()
{

    if(channel1 + 1 != channel2){
    channel1++;
    }else{
        channel1 += 2;
    }
    update();
    //emit up for primary signal here
}

void ChannelSelectionView::up2Clicked()
{
    if(channel2 + 1 != channel1){
    channel2++;
    }else{
        channel2 += 2;
    }
    update();
    //emit up for secondary signal here
}

void ChannelSelectionView::down1Clicked()
{
    if(channel1 > 0 && (channel1 - 1) != channel2){
    channel1--;
    update();
    //emit down for primary signal here
    }else if((channel1 - 2) > 0){
        channel1 -= 2;
    }else{
        qDebug() << "Cannot go down a channel in primary";
    }
}

void ChannelSelectionView::down2Clicked()
{
    if(channel2 > 0 && (channel2 -1) != channel1){
        channel2--;
        update();
         //emit down for secondary signal here
    }else if((channel2 - 2) > 0){
        channel2 -= 2;
    }else{
        qDebug() << "Cannot go down a channel in secondary";
    }

}

void ChannelSelectionView::setPrimaryChan(int channel)
{
    channel1 = channel;
}

void ChannelSelectionView::setSecondaryChan(int channel)
{
    channel2 = channel;
}
