#include "cvmatviewer.h"

/*bug #0 : delay to pop up menu's after paused for a period of time and menu is closed*/
/*bug #1 : openCV runs out of memory when storing too many frames in data structure, need to figure out how to fix*/

cvMatViewer::cvMatViewer( QWidget *parent) :
    QWidget(parent)
{

}

cvMatViewer::~cvMatViewer()
{

}

QSize cvMatViewer::sizeHint() const
{
    return qimg.size();
}

QSize cvMatViewer::minimumSizeHint() const
{
    return qimg.size();
}


void cvMatViewer::showImg(const Mat &img)
{


    switch(img.type()){

    case CV_8UC1:
        cvtColor(img, mat,CV_GRAY2RGB);
        break;

    case CV_8UC3:
        cvtColor(img, mat,CV_BGR2RGB);
        break;

    }


   qimg = QImage(mat.data, mat.cols, mat.rows, mat.cols*3, QImage::Format_RGB888);
   this->setFixedSize(img.cols,img.rows);

   repaint();


}





void cvMatViewer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0,0), qimg);
    painter.end();
}
