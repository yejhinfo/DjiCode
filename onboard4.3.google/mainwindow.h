#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <iostream>
#include <fstream>
#include <QItemDelegate>
#include <QMutex>
#include <QThread>
#include <QComboBox>
#include <QTextBrowser>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QTime>
#include <QTimer>
#include <QDir>
#include <string>
#include <QWebEngineView>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public:
    int CurrentIndex;

    uint8_t flightFlag;
    float_t flightX=0;
    float_t flightY=0;
    float_t flightZ=0;
    float_t flightYaw=0;
    QString MsData;
    QByteArray MsData1;
    QString RemoveFlag=0;
    int point_count = 0;

private:
    void handleXYRanges(double minXY, double maxXY);
    void handleZRanges(double minZ, double maxZ);
    void handleYawRanges(double minYaw, double maxYaw);

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_btn_coreSetControl_clicked();
    void on_btn_flight_runTask_clicked();

    void on_lineEdit_flight_X_returnPressed();

    void on_lineEdit_flight_Y_returnPressed();

    void on_lineEdit_flight_Z_returnPressed();

    void on_lineEdit_flight_YAW_returnPressed();

    void on_btn_flight_send_clicked();


    void on_tn_waypoint_add_clicked();

    void on_btn_waypoint_init_clicked();

    void on_btn_waypoint_remove_clicked();

    void on_btn_wp_loadAll_clicked();



    void on_btn_wp_start_stop_clicked(bool checked);

    void on_btn_AbortWaypoint_clicked();

    void on_btn_wp_pr_clicked(bool checked);

    void on_openButton_clicked();

    void on_waypoint_data_changed(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                  const QVector<int> &roles );
    void on_btn_Locate_clicked();

    void on_btn_Restart_clicked();


    void on_btn_Plan_clicked();

    void on_btn_Startplan_clicked();

    void on_pushButton_3_clicked();


    void on_TakeOff_clicked();

    void on_Landing_clicked();


    void on_buttonGroupHorizontal_buttonClicked(QAbstractButton* button);
    void on_buttonGroupVertical_buttonClicked(QAbstractButton* button);
    void on_buttonGroupYaw_buttonClicked(QAbstractButton* button);
    void on_buttonGroupFrame_buttonClicked(QAbstractButton* button);
    void on_buttonGroupStable_buttonClicked(QAbstractButton* button);

    void on_xControlSpinBox_valueChanged(double arg1);
    void on_xControlSlider_valueChanged(int value);

    void on_yControlSpinBox_valueChanged(double arg1);
    void on_yControlSlider_valueChanged(int value);

    void on_zControlSpinBox_valueChanged(double arg1);
    void on_zControlSlider_valueChanged(int value);

    void on_yawControlSpinBox_valueChanged(double arg1);
    void on_yawControlSlider_valueChanged(int value);



    void on_portListGet_clicked();

    void on_Flightstart_clicked();


    
    void on_Ending_clicked();

    void on_Flying_clicked();

    void on_GoHome_clicked();

public slots:
    void AddMapPOint(QString lon ,QString lat);
    void AddMissionPOint(QString lon ,QString lat ,QString FinshFlag);
    void RemoveAll();
public:

    void on_btn_coreActive_clicked();
    void SetPortList(QString ListData);
    void flightSend();
    void Read_Data();
    void initWayPoint();
    void sleep(unsigned int msec);
private:
    Ui::MainWindow *ui;
    QStandardItemModel *waypointData;
    QSerialPort *serial;
    QWebEngineView *view;




};

#endif // MAINWINDOW_H
