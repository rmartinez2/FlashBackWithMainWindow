#include "frameglwidget.h"

FrameGLWidget::FrameGLWidget(QWidget *parent):
    QGLWidget(QGLFormat(QGL::SampleBuffers),parent)
{



}

void FrameGLWidget::showImg(Mat img)
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

void FrameGLWidget::paintEvent(QPaintEvent *a)
{
    Q_UNUSED(a);
    QPainter painter(this);

    painter.drawImage(QPoint(0,0), qimg);

    painter.end();
}





