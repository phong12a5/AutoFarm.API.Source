#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <iostream>
#include <stdio.h>
#include <QObject>
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
