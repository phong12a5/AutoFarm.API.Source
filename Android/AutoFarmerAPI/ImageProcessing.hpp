#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <iostream>
#include <stdio.h>
#include <QObject>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include "QPoint"


class ImageProcessing : public QObject
{
    Q_OBJECT
private:
    explicit ImageProcessing(QObject *parent = nullptr);

public:
    static QPoint findImageOnImage(const QString templatePath, const QString imagePath, const float threshold, float scale);
    static float getThreshhold();
    static float getScale();

public slots:
};

#endif // IMAGEPROCESSING_H
