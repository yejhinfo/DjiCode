#ifndef BRIDGE_H
#define BRIDGE_H

#include <QObject>
#include <QJSValue>
#include <functional>
#include <QVariant>

class bridge : public QObject
{
Q_OBJECT
public:
    bridge(QObject *parent);
    ~bridge();

signals:
    void sigYourName(const QString &name);
    void AddMapPoint(QString lat ,QString lon);
    void AddMissionPoint(QString lat ,QString lon ,QString FinshFlag);
public slots:
    void showMsgBox();
    void getYourName();
    Q_INVOKABLE void getCoordinate(QString lon,QString lat);
    Q_INVOKABLE void getMissionPoint(QString lon, QString lat, QString FinshFlag);
private:

};

#endif // BRIDGE_H
