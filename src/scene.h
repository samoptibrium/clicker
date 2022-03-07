#pragma once

#include <QGraphicsScene>
#include <QPixmap>

namespace clickeroo
{

class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit Scene(QObject* parent);
    ~Scene();

    void setCaptureGraphic(const QPixmap& graphic);

private:
    QGraphicsPixmapItem* currentGraphic{ nullptr };
};

}


