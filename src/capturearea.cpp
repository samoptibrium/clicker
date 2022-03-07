#include "capturearea.h"

#include <cassert>
#include <cmath>
#include <optional>

#include <QPixmap>
#include <QRect>

namespace
{

QRect captureRect(0, 0, 100, 100);
std::optional<QImage> lastGrabbedImage = std::nullopt;

double colourDistance(const QRgb e1, const QRgb e2)
{
    long rmean = ( (long)qRed(e1) + (long)qRed(e2) ) / 2;
    long r = (long)qRed(e1) - (long)qRed(e2);
    long g = (long)qGreen(e1) - (long)qGreen(e2);
    long b = (long)qBlue(e1) - (long)qGreen(e2);
    return std::sqrt((((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8));
}

int toImageX(double percentX)
{
    if(!lastGrabbedImage) {
        return 0;
    }
    return std::round((double)(lastGrabbedImage->width()) * (percentX / 100.0));
}

int toImageY(double percentY)
{
    if(!lastGrabbedImage) {
        return 0;
    }
    return std::round((double)(lastGrabbedImage->height()) * (percentY / 100.0));
}

}

namespace clickeroo
{
    
QRect& getCaptureArea()
{
    return ::captureRect;
}

void setCaptureArea(const QRect& rect)
{
    ::captureRect = rect;
}

QImage getImage()
{
    if(!::lastGrabbedImage) {
        return QImage();
    }
    return ::lastGrabbedImage.value();
}

void setImage(QImage image)
{
    ::lastGrabbedImage = image;
}

unsigned int getRgbPx(int x, int y)
{
    if(!lastGrabbedImage) {
        return 0;
    }
    if(!getImage().valid(x, y)) {
        //assert(0 && "Trying to grab pixel at invalid coordinate");
        return 0;
    }
    return getImage().pixel(x, y);
}

double colourDiffPixelPx(int x, int y, int r, int g, int b)
{
    if(!lastGrabbedImage) {
        assert(0 && "Trying to grab pixel from non-existent image");
        return 0;
    }
    return colourDistance(qRgb(r, g, b), getRgbPx(x, y));
}

double colourDiffAreaPx(int x1, int y1, int x2, int y2, int r, int g, int b)
{
    double sumDiff = 0;
    double pxCount = 0;
    for(int x = x1; x <= x2; x++) {
        for(int y = y1; y <= y2; y++) {
            sumDiff += colourDiffPixelPx(x, y, r, g, b);
            pxCount++;
        }
    }
    return sumDiff / pxCount;
}

unsigned int getRgb(double x, double y)
{
    return getRgbPx(toImageX(x), toImageY(y));
}

double colourDiffPixel(double x, double y, int r, int g, int b)
{
    return colourDiffPixelPx(toImageX(x), toImageY(y), r, g, b);
}

double colourDiffArea(double x1, double y1, double x2, double y2, int r, int g, int b)
{
    return colourDiffAreaPx(toImageX(x1), toImageY(y1), toImageX(x2), toImageY(y2),r, g, b);
}

bool testAreaDiffLessThan(double x1, double y1, double x2, double y2, int r, int g, int b, double diffThreshold)
{
    return colourDiffArea(x1, y1, x2, y2, r, g, b) < diffThreshold;
}

double percentToMouseX(const double x)
{
    return captureRect.x() + (captureRect.width() * x / 100.0);
}

double percentToMouseY(const double y)
{
    return captureRect.y() + (captureRect.height() * y / 100.0);
}

}
