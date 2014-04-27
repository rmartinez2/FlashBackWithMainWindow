#ifndef FRAMEGLWIDGET_H
#define FRAMEGLWIDGET_H


#include <qgl.h>

#include "opencv/cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <QtCore>
#include <QtDebug>
#include <QImage>
#include <QColor>
#include <QPainter>

using namespace cv;
using namespace std;


class FrameGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit FrameGLWidget(QWidget *parent = 0);

    QImage qimg;
    Mat mat;

private:


    
signals:


    
public slots:
    void showImg(Mat img);


protected:
    void paintEvent(QPaintEvent *a);


    
};

#endif // FRAMEGLWIDGET_H
