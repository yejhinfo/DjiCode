#include "bridge.h"
#include <QMessageBox>
#include <QJSEngine>
#include <QDebug>

bridge::bridge(QObject *parent)
{

}

bridge::~bridge()
{
}

void bridge::showMsgBox()
{
    QMessageBox::aboutQt(0, tr("Qt"));
}

void bridge::getYourName()
{
    emit sigYourName("hello, world!");
}

void bridge::getCoordinate(QString lon, QString lat)
{
    qDebug()<<lon<<lat;
    emit AddMapPoint(lon,lat);
}

void bridge::getMissionPoint(QString lon, QString lat, QString FinshFlag)
{
    //qDebug()<<lon<<lat;

    emit AddMissionPoint(lon,lat,FinshFlag);
}
