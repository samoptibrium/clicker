#pragma once

#include <QPixmap>
#include <QRect>

namespace clickeroo
{

QRect& getCaptureArea();
void setCaptureArea(const QRect& rect);

QImage getImage();
void setImage(QImage image);

unsigned int getRgbPx(int x, int y);
double colourDiffPixelPx(int x, int y, int r, int g, int b);
double colourDiffAreaPx(int x1, int y1, int x2, int y2, int r, int g, int b);

unsigned int getRgb(double x, double y);
double colourDiffPixel(double x, double y, int r, int g, int b);
double colourDiffArea(double x1, double y1, double x2, double y2, int r, int g, int b);

bool testAreaDiffLessThan(double x1, double y1, double x2, double y2, int r, int g, int b, double diffThreshold);

double percentToMouseX(double x);
double percentToMouseY(double y);

}
