#include "scene.h"

#include <QGraphicsPixmapItem>

namespace clickeroo
{
    
Scene::Scene(QObject* parent) : QGraphicsScene(parent)
{
}

Scene::~Scene()
{
}

void Scene::setCaptureGraphic(const QPixmap& graphic)
{
    if(!currentGraphic) {
        currentGraphic = addPixmap(graphic);
        setSceneRect(itemsBoundingRect());
        return;
    }
    currentGraphic->setPixmap(graphic);
    setSceneRect(itemsBoundingRect());
}

}
