#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWebChannel>   //实现js与qt交互必需
#include <QDir>
#include <QTime>
#include "bridge.h"      //所有被js调用的函数，必须在此头文件登记

#include <iostream>
#include <string>

#include "myClient.hpp"  // 网络通信

int flightTask;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //查找可用的串口

    view = new QWebEngineView(ui->widget);    //新建类，显示地图
    QDir temDir("G:/pro2/pro2/ProCp/onboard4.3.google/GoogleMapsAPIv3/sample.html");
    QString absDir = temDir.absolutePath();
    QWebChannel *channel = new QWebChannel(this);
    bridge *pbridge = new bridge(this);
    channel->registerObject("bridge", pbridge);  //"bridge" 为bridge这个类在js里的登记名
    view->page()->setWebChannel(channel);
    view->page()->load(QUrl(QString("file:///%0").arg(absDir)));   
    //以上均与js交互相关

    connect(pbridge,SIGNAL(AddMapPoint(QString , QString)),this,SLOT(AddMapPOint(QString,QString)));//实现值传递
    connect(pbridge,SIGNAL(AddMissionPoint(QString , QString, QString)),this,SLOT(AddMissionPOint(QString,QString,QString)));
    initWayPoint();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        qDebug()<<"Name       :"<<info.portName();
        qDebug()<<"Description:"<<info.description();
        qDebug()<<"Manufaction:"<<info.manufacturer();
        qDebug()<<"Serial Number:"<<info.serialNumber();
        qDebug()<<"System Location:"<<info.systemLocation();
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->PortBox->addItem(serial.portName());
            serial.close();
        }
    }

    //initSocket();
    ui->btn_Startplan->setEnabled(false);
    qDebug() << tr("Succeed initialize Display.");
}

MainWindow::~MainWindow()
{
    delete ui;
}


// 获取无人机端串口列表
void MainWindow::on_portListGet_clicked()
{
    serial->write(tr("PortList!\r\n").toLatin1());
}

void MainWindow::SetPortList(QString ListData)
{
    QString currentPort;
    currentPort = ui->portSelection->currentText();

    int PortNumber;
    QStringList Port_List;
    QString Port_name;
    PortNumber = ListData.section("!",1,1).toInt();
    for (int i=2;i<PortNumber+2; i++)
    {
        Port_name = ListData.section("!",i,i);
        Port_List.append(Port_name);
    }

    if (PortNumber== 0)
        Port_List.append("Connect Serial");

    ui->portSelection->addItems(Port_List);

    if (PortNumber != 0)
    {
      ui->portSelection->setCurrentIndex(ui->portSelection->findText(currentPort));
    }

    if (currentPort.isEmpty())
    {
      ui->portSelection->setCurrentIndex(0);
    }
}


// 激活按钮
void MainWindow::on_pushButton_2_clicked()
{
     serial->write(tr("active!!!!!!!!!\r\n").toLatin1());
}

// 初始化按钮
void MainWindow::on_pushButton_clicked()
{
    // 发送指令和端口号
    QString port_name="init!!!!!!!!!!!\r\n";
    port_name += ui->portSelection->currentText() + "!\r\n";
    qDebug() << port_name;
    serial->write(port_name.toLatin1());
}


// 获取控制权按钮
void MainWindow::on_btn_coreSetControl_clicked()
{
     serial->write(tr("OControl!!!!!!!\r\n").toLatin1());
}


void MainWindow::sleep(unsigned int msec)
{
QTime dieTime = QTime::currentTime().addMSecs(msec);
while( QTime::currentTime() < dieTime )
QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


void MainWindow::on_btn_flight_runTask_clicked()
{
   QByteArray task;
   task="GoHome!\r\n";
//   if(ui->radioTakeOff->isChecked())
//       task="Takeoff!\r\n";
//   else if(ui->radioLanding->isChecked())
//       task="Land!\r\n";
//   else if(ui->radioGoHome->isChecked())
//       task="GoHome!\r\n";
   serial->write(tr(task).toLatin1());

}

//当QStandardItemModel这个类（即mission里的经纬度栏）里面的数据发生时会触发这个槽，返回被改变数值的列，行和改变数值
void MainWindow::on_waypoint_data_changed(const QModelIndex &topLeft ,
                                             const QModelIndex &bottomRight ,
                                             const QVector<int> &roles )
{
    //! @todo waypoint data modify
    QString NData;
    QByteArray data2;
    QString data1=QString("%1").arg(bottomRight.column());//column列　row行
    NData= "NData!";
    NData +=data1+"!";//arg方法类似printf，QString(%1)表示输出一个
    data1=QString("%1").arg(bottomRight.row());
    NData +=data1+"!";
    data1=QString("%1").arg(topLeft.data().toDouble(),0,'f',6);
    NData +=data1+"!\r\n";
    data2=NData.toLatin1();
    qDebug() << NData;
    serial->write(tr(data2).toLatin1());
    //将改变的数值告诉接收端，使接收端修改相应数值

}

void MainWindow::on_lineEdit_flight_X_returnPressed()
{
    //flightX = ui->lineEdit_flight_X->text().toFloat();
}

void MainWindow::on_lineEdit_flight_Y_returnPressed()
{
    //flightY = ui->lineEdit_flight_Y->text().toFloat();
}

void MainWindow::on_lineEdit_flight_Z_returnPressed()
{
    //flightZ = ui->lineEdit_flight_Z->text().toFloat();
}

void MainWindow::on_lineEdit_flight_YAW_returnPressed()
{
    //flightYaw = ui->lineEdit_flight_YAW->text().toFloat();
}

void MainWindow::on_btn_flight_send_clicked()
{
    flightSend();
}


// 获取无人机端口串口号列表，用于初始化无人机、激活、获取控制权





// waypoint 类型 如下

void MainWindow::initWayPoint()
{
    waypointData = new QStandardItemModel ;
    waypointData ->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("No.")));
    waypointData->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Latitude")));
    waypointData->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("Longitude")));
    waypointData->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("Altitude")));
//    ui->tv_waypoint_data->setItemDelegateForColumn(0, new ReadOnlyDelegate());
    ui->tv_waypoint_data->setModel(waypointData);
    ui->tv_waypoint_data->verticalHeader()->hide();  //yin cang hangtou
    ui->tv_waypoint_data->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//pingjunfenpeiliekuang
//    connect(waypointData, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this,
//            SLOT(on_waypoint_data_changed(QModelIndex, QModelIndex, QVector<int>)));
    ui->cb_waypoint_point->addItem("Null");
}
void MainWindow::on_tn_waypoint_add_clicked()
{
    QByteArray Mission;
    Mission="Wp!Add!\r\n";
    serial->write(tr(Mission).toLatin1());
}

void MainWindow::on_btn_waypoint_init_clicked()
{
    QByteArray Mission;
    Mission="WpInit!!!!!!!!!\r\n";
    serial->write(tr(Mission).toLatin1());
}

void MainWindow::on_btn_waypoint_remove_clicked()
{
    QByteArray Mission;
    Mission="OpRemove!!!!!!!\r\n";
    serial->write(tr(Mission).toLatin1());
    if (ui->cb_waypoint_point->count() != 1)
    {
        ui->cb_waypoint_point->removeItem(ui->cb_waypoint_point->count() - 1);

        waypointData->removeRow(waypointData->rowCount() - 1);
    }
    ui->le_waypoint_number->setText(QString::number(waypointData->rowCount()));
}
void MainWindow::RemoveAll()
{
    QByteArray Mission;
    Mission="Wp!ReAll!!!!!!!\r\n";
    serial->write(tr(Mission).toLatin1());
    int number=waypointData->rowCount();
    int i;
    for(i=0;i<number;i++)
    {
    if(ui->cb_waypoint_point->count() != 1)
    {
        ui->cb_waypoint_point->removeItem(ui->cb_waypoint_point->count() - 1);

        waypointData->removeRow(waypointData->rowCount() - 1);
    }
    ui->le_waypoint_number->setText(QString::number(waypointData->rowCount()));
    }
}

void MainWindow::on_btn_wp_loadAll_clicked()
{
    QByteArray Mission;
    Mission="WpLoadAll!!!!!!\r\n";
    serial->write(tr(Mission).toLatin1());
}


void MainWindow::on_btn_wp_start_stop_clicked(bool checked)
{
    QByteArray Mission;
    Mission="WpStart!!!!!!!!\r\n";
    serial->write(tr(Mission).toLatin1());
}

void MainWindow::on_btn_AbortWaypoint_clicked()
{
    QByteArray Mission;
    Mission="Wp!Abort!!!!!!!\r\n";
    serial->write(tr(Mission).toLatin1());
}

void MainWindow::on_btn_wp_pr_clicked(bool checked)
{

}

void MainWindow::on_openButton_clicked()
{

    if(ui->openButton->text()==tr("OpenSerial") || ui->openButton->text()==tr("打开串口"))
    {
        serial = new QSerialPort;
        //设置串口名
        serial->setPortName(ui->PortBox->currentText());
        //打开串口
        serial->open(QIODevice::ReadWrite);
        //设置波特率
        serial->setBaudRate(QSerialPort::Baud9600);
        //设置数据位数
        serial->setDataBits(QSerialPort::Data8);
        //设置奇偶校验
        serial->setParity(QSerialPort::NoParity);
        //设置停止位
        serial->setStopBits(QSerialPort::OneStop);
        //设置流控制
        serial->setFlowControl(QSerialPort::NoFlowControl);

        //关闭设置菜单使能
        ui->PortBox->setEnabled(false);
        ui->openButton->setText(tr("CloseSerial"));
        //连接信号槽
        QObject::connect(serial, &QSerialPort::readyRead, this, &MainWindow::Read_Data);

        qDebug() << tr("Succeed Open Serial.");
    }
    else
    {
        /*
        //关闭串口
        serial->clear();
        serial->close();
        serial->deleteLater();
        */
        //恢复设置使能
        ui->PortBox->setEnabled(true);
        ui->openButton->setText(tr("OpenSerial"));

        qDebug() << tr("Succeed Close Serial.");
    }
}
//读取接收到的数据, 在无人机端，编写一个类似的函数实现接收数据功能
void MainWindow::Read_Data()
{
    static QByteArray buf;
    QString RecData,DeaData;
    buf += serial->readAll();
    if(buf.endsWith("\r\n"))  //检测到结束符/r/n
    {
         RecData=tr(buf);  // 存储读取到的数据
         RecData=RecData.left(RecData.indexOf("\r\n")); // 切片操作
         ui->textEdit->append(RecData);  // 把数据在数据展示区中显示出来
         DeaData=RecData.section("!",0,0);
         qDebug() << RecData;
         buf.clear();   //clear  清空buf接收区
         // 如果是航点数据，则是wpdata
         if(DeaData=="wpdata")
         {   int number;
             double Latitude,Longitude,Altitude;
             disconnect(waypointData, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this,
                     SLOT(on_waypoint_data_changed(QModelIndex, QModelIndex, QVector<int>)));//数据改变不会触发槽
             DeaData=RecData.section("!",1,1);
             number=DeaData.toInt();
             DeaData=RecData.section("!",2,2);
             Latitude=DeaData.toFloat();
             DeaData=RecData.section("!",3,3);
             Longitude=DeaData.toFloat();
             DeaData=RecData.section("!",4,4);
             Altitude=DeaData.toFloat();
             waypointData->setItem(number, 0, new QStandardItem(QString::number(number)));
             waypointData->setItem(number, 1,
                                   new QStandardItem(QString::number(Latitude,'f',6)));
             waypointData->setItem(number, 2,
                                   new QStandardItem(QString::number(Longitude,'f',6)));
             waypointData->setItem(number, 3,
                                   new QStandardItem(QString::number(Altitude,'f',6)));
             connect(waypointData, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this,
                     SLOT(on_waypoint_data_changed(QModelIndex, QModelIndex, QVector<int>)));//在add waypoint结束后，若数据改变会触发槽
             ui->le_waypoint_number->setText(QString::number(waypointData->rowCount()));
             ui->cb_waypoint_point->addItem(QString::number(ui->cb_waypoint_point->count() - 1));
         }
         // 获取控制权的命令
         if(DeaData=="OcontrolOk")
         {
            ui->btn_coreSetControl->setText("Release Control");
         }
         // 释放控制权的命令
         if(DeaData=="RcontrolOk")
         {
            ui->btn_coreSetControl->setText("Obtain Control");
         }
         // 获取飞机自身的位置
         if(DeaData=="LOdata")
         {
            QString Latitude,Longitude;
            Latitude  = RecData.section("!",1,1);
            Longitude = RecData.section("!",2,2);
            view->page()->runJavaScript(QString("showAlert('%1','%2');").arg(Longitude).arg(Latitude));
         }
         if(DeaData=="Re")
         {
            QString interval;
            interval = ui->le_interval->text();
            view->page()->runJavaScript(QString("PlanStart('%1');").arg(interval));
         }
         if(DeaData=="PortList")
         {
             SetPortList(RecData);
         }
    }
}

void MainWindow::on_btn_Locate_clicked()//定位飞机位置
{
     QString lon,lan;
     lon="113.33929815219336";
     lan="23.16283848723737";

     view->page()->runJavaScript(QString("showAlert('%1','%2');").arg(lon).arg(lan));
     serial->write(tr("locate!!!!!!!!!\r\n").toLatin1());
}

void MainWindow::on_btn_Restart_clicked()
{
     RemoveAll();
     view->page()->runJavaScript(QString("initialize();"));
        //view->page()->runJavaScript(QString("plantline();"));
       //view->page()->runJavaScript(QString("getdistance1();"));
}

void MainWindow::AddMapPOint(QString lat ,QString lon)//在地图上单点加入waypoint
{
    if(ui->Flightstart->isEnabled()==true)
    {
        qDebug() << "Press Flight start button first!";
        return;
    }
     // 转换格式
     double lon_val, lat_val;
     lon_val = lon.toDouble() * 3.1415926535898 / 180;

     lon=QString("%1").arg(QString::number(lon_val, 'f', 7));
     lat_val = lat.toDouble() * 3.1415926535898 / 180;
     lat=QString("%1").arg(QString::number(lat_val, 'f', 7));
     qDebug()<<lon<<lat;

     QString MapData;
     QByteArray MapData1;
     disconnect(waypointData, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this,
             SLOT(on_waypoint_data_changed(QModelIndex, QModelIndex, QVector<int>)));
     int number = waypointData->rowCount();
     waypointData->setItem(number, 0, new QStandardItem(QString::number(number)));
     waypointData->setItem(number, 1,
                           new QStandardItem(lat));
     waypointData->setItem(number, 2,
                           new QStandardItem(lon));
     waypointData->setItem(number, 3,
                           new QStandardItem(ui->le_Altitude->text()));
     ui->le_waypoint_number->setText(QString::number(waypointData->rowCount()));
     ui->cb_waypoint_point->addItem(QString::number(ui->cb_waypoint_point->count() - 1));
     connect(waypointData, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this,
             SLOT(on_waypoint_data_changed(QModelIndex, QModelIndex, QVector<int>)));
     //MapData="Op!";
     //MapData +=QString("%1").arg(number)+"!";
     QString numCh;
     MapData = "la";
     numCh = QString::QString((10-lat.length()), '!');
     MapData = MapData + QString("%1").arg(point_count,2,10, QLatin1Char('0')) + ":" + lat + numCh + "\r\n";
     serial->write(tr(MapData.toLatin1()).toLatin1());
     qDebug() << MapData;



     numCh = QString::QString((10-lon.length()), '!');
     MapData = "lo";
     MapData = MapData + QString("%1").arg(point_count,2,10, QLatin1Char('0')) + ":" + lon + numCh + "\r\n";
     //MapData +=ui->le_Altitude->text()+"!";
     //MapData +=QString("%1").arg(number)+"!\r\n";

     serial->write(tr(MapData.toLatin1()).toLatin1());
     qDebug() << MapData;

     point_count += 1;
     //MapData1=MapData.toLatin1();
     //serial->write(tr(MapData1).toLatin1());


}

//QString WpData;
QString MsData="";
QByteArray MsData1;
void MainWindow::AddMissionPOint(QString lat ,QString lon ,QString FinshFlag)//规划航线,逐次加入，finshflag为结束标识
{
    QString MapData;
    // 转换
    double lon_val, lat_val;
    lon_val = lon.toDouble() * 3.1415926535898 / 180;
    lon=QString("%1").arg(QString::number(lon_val, 'f', 7));
    lat_val = lat.toDouble() * 3.1415926535898 / 180;
    lat=QString("%1").arg(QString::number(lat_val, 'f', 7));

    //lon=lon.left(10);  // 字符串左边10位
    //lat=lat.left(9);
    qDebug()<<lon<<lat; // 这个和brige里面那个是否重复？


    if(RemoveFlag=="1")
    {
        RemoveAll();
        RemoveFlag="0";
        sleep(20);
    }
    //qDebug() << tr("I'm here");
    disconnect(waypointData, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this,
            SLOT(on_waypoint_data_changed(QModelIndex, QModelIndex, QVector<int>)));
    //qDebug() << tr("I'm here2");
    int number = waypointData->rowCount();
    waypointData->setItem(number, 0, new QStandardItem(QString::number(number)));
    waypointData->setItem(number, 1,
                          new QStandardItem(lat));
    waypointData->setItem(number, 2,
                          new QStandardItem(lon));
    waypointData->setItem(number, 3,
                          new QStandardItem(ui->le_Altitude->text()));
    ui->le_waypoint_number->setText(QString::number(waypointData->rowCount()));
    ui->cb_waypoint_point->addItem(QString::number(ui->cb_waypoint_point->count() - 1));
    connect(waypointData, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this,
            SLOT(on_waypoint_data_changed(QModelIndex, QModelIndex, QVector<int>)));
    //qDebug() << tr("I'm here3");

    //MsData +=QString("%1").arg(number)+"!";
    //MsData +=lat+"!";
    //MsData +=lon+"!";

    // 分经度 纬度 发送
    QString numCh;
    MapData = "la";
    numCh = QString::QString((10-lat.length()), '!');
    MapData = MapData + QString("%1").arg(point_count,2,10, QLatin1Char('0')) + ":" + lat + numCh + "\r\n";
    serial->write(tr(MapData.toLatin1()).toLatin1());
    qDebug() << MapData;

    numCh = QString::QString((10-lon.length()), '!');
    MapData = "lo";
    MapData = MapData + QString("%1").arg(point_count,2,10, QLatin1Char('0')) + ":" + lon + numCh + "\r\n";
    serial->write(tr(MapData.toLatin1()).toLatin1());
    qDebug() << MapData;

    point_count += 1;


    //qDebug() << tr("WayPoint mission: Add waypoint: No.") + QString("%1").arg(number);
    //qDebug() << number;
    if(FinshFlag=="1")//规划航线结束后，若再次进入则删除原来航线
    {
        // send all waypoints together
        // MsData +=FinshFlag+"!";
        // MsData +=ui->le_Altitude->text()+"!\r\n";
        // MsData = "Wp!" + QString("%1").arg(number) + "!" + ui->le_Altitude->text() + "!" + MsData;
        // MsData1=MsData.toLatin1();
        // serial->write(tr(MsData1).toLatin1());
        //serial->write();
       RemoveFlag="1";
       // MsData = "";



       qDebug() << "Finish sendding points";
    }
}

// plan 按钮
void MainWindow::on_btn_Plan_clicked()
{
      if(ui->btn_Plan->text()==tr("plan"))//转为单点模式
      {
          ui->btn_Startplan->setEnabled(true);
          ui->btn_Plan->setText(tr("OnePoint"));
          view->page()->runJavaScript(QString("PlanModel();"));

      }
      else//转为规划模式
      {
          ui->btn_Startplan->setEnabled(false);
          ui->btn_Plan->setText(tr("plan"));
          view->page()->runJavaScript(QString("PlanModel();"));
      }
}

void MainWindow::on_btn_Startplan_clicked()//发送规划航线的间隔
{
    QString interval;
    interval = ui->le_interval->text();
    view->page()->runJavaScript(QString("PlanStart('%1');").arg(interval));
}


void MainWindow::on_pushButton_3_clicked()
{
//    QString mycmd;
//    mycmd = ui->lineEdit->text();
//    QByteArray mycmd1;
//    mycmd1 = mycmd.toLatin1();
//    char bcode[] = "cmd ok";
//    SendCMD(mycmd1.data(), bcode);

}


// Flight mission params sending settings




void MainWindow::on_TakeOff_clicked()
{
    //flightTask = Control::FlightCommand::takeOff;
    //qDebug() << ui->cb_command->currentIndex() << flightTask;
    //vehicle->control->action(flightTask, MainWindow::actionCallback,
    //                         this);

    serial->write(tr("takeoff!!!!!!!!\r\n").toLatin1());
}


void MainWindow::on_Landing_clicked()
{
    //flightTask = Control::FlightCommand::takeOff;
    //qDebug() << ui->cb_command->currentIndex() << flightTask;
    //vehicle->control->action(flightTask, MainWindow::actionCallback,
    //                         this);

    serial->write(tr("Land!!!!!!!!!!!\r\n").toLatin1());
}


QString flight_data;


void
MainWindow::on_buttonGroupHorizontal_buttonClicked(
  QAbstractButton* button)
{
  QString name = button->text();
  flight_data += name+"!";
  //command.flag &= 0x3F;
  if (name == "Angle")
  {
    //command.flag |= Control::HorizontalLogic::HORIZONTAL_ANGLE;
    ui->xControlModeText->setText("Roll (deg):");
    ui->yControlModeText->setText("Pitch (deg):");
    this->handleXYRanges(-30, 30);
  }
  else if (name == "Velocity")
  {
    //command.flag |= Control::HorizontalLogic::HORIZONTAL_VELOCITY;
    ui->xControlModeText->setText("X Vel (m/s):");
    ui->yControlModeText->setText("Y Vel (m/s):");
    this->handleXYRanges(-10, 10);
  }
  else if (name == "Position")
  {
    //command.flag |= Control::HorizontalLogic::HORIZONTAL_POSITION;
    ui->xControlModeText->setText("X Offset (m):");
    ui->yControlModeText->setText("Y Offset (m):");
    this->handleXYRanges(-10, 10);
  }
  else if (name == "Angular Rate")
  {
    //command.flag |= Control::HorizontalLogic::HORIZONTAL_ANGULAR_RATE;
    ui->xControlModeText->setText("Roll rate (deg/s):");
    ui->yControlModeText->setText("Pitch rate (deg/s):");
    this->handleXYRanges(-50, 50);
  }
}

void
MainWindow::on_buttonGroupVertical_buttonClicked(
  QAbstractButton* button)
{
  QString name = button->text();
  flight_data += name + "!";
  //command.flag &= 0xCF;
  if (name == "Thrust")
  {
    //command.flag |= Control::VerticalLogic::VERTICAL_THRUST;
    ui->zControlModeText->setText("Z Thr (%):");
    this->handleZRanges(0, 100);
    ui->zControlSlider->setValue(25);
  }
  else if (name == "Velocity")
  {
    //command.flag |= Control::VerticalLogic::VERTICAL_VELOCITY;
    ui->zControlModeText->setText("Z Vel (m/s):");
    this->handleZRanges(-5, 5);
  }
  else if (name == "Position")
  {
    //command.flag |= Control::VerticalLogic::VERTICAL_POSITION;
    ui->zControlModeText->setText("Z Abs Pos (m):");
    this->handleZRanges(0, 120);
    ui->zControlSlider->setValue(1.2);
  }
}

void
MainWindow::on_buttonGroupYaw_buttonClicked(QAbstractButton* button)
{
  QString name = button->text();
  flight_data += name + "!";
  //command.flag &= 0xF7;
  if (name == "Yaw Angle")
  {
    //command.flag |= Control::YawLogic::YAW_ANGLE;
    ui->yawControlModeText->setText("Yaw (deg):");
    this->handleYawRanges(-180, 180);
  }
  else
  {
    //command.flag |= Control::YawLogic::YAW_RATE;
    ui->yawControlModeText->setText("Yaw rate (deg/s):");
    this->handleYawRanges(-100, 100);
  }
}

void
MainWindow::on_buttonGroupFrame_buttonClicked(QAbstractButton* button)
{
  QString name = button->text();
  flight_data += name + "!";
  //command.flag &= 0xF9;
  if (name == "Ground")
  {
    //command.flag |= Control::HorizontalCoordinate::HORIZONTAL_GROUND;
  }
  else
  {
    //command.flag |= Control::HorizontalCoordinate::HORIZONTAL_BODY;
  }
}

void
MainWindow::on_buttonGroupStable_buttonClicked(QAbstractButton* button)
{
  //if (vehicle->getFwVersion() > Version::FW(3, 1, 0, 0))
  //{
    QString name = button->text();
    flight_data += name + "!";
    //command.flag &= 0xFE;
    if (name == "Disable")
    {
      //command.flag |= Control::StableMode::STABLE_DISABLE;
    }
    else
    {
      //command.flag |= Control::StableMode::STABLE_ENABLE;
    }

  //else
  //{
    //ui->groupBox9->setDisabled(true);
  //}
}

void
MainWindow::on_xControlSpinBox_valueChanged(double arg1)
{
  if (ui->xControlSlider->value() != (int)arg1)
  {
    ui->xControlSlider->setValue((int)arg1);
  }
  //this->command.x = arg1;
}

void
MainWindow::on_xControlSlider_valueChanged(int value)
{
  ui->xControlSpinBox->setValue(value);
}

void
MainWindow::on_yControlSpinBox_valueChanged(double arg1)
{
  if (ui->yControlSlider->value() != (int)arg1)
  {
    ui->yControlSlider->setValue((int)arg1);
  }
  //this->command.y = arg1;
}

void
MainWindow::on_yControlSlider_valueChanged(int value)
{
  ui->yControlSpinBox->setValue(value);
}

void
MainWindow::on_zControlSpinBox_valueChanged(double arg1)
{
  if (ui->zControlSlider->value() != (int)arg1)
  {
    ui->zControlSlider->setValue((int)arg1);
  }
  //this->command.z = arg1;
}

void
MainWindow::on_zControlSlider_valueChanged(int value)
{
  ui->zControlSpinBox->setValue(value);
}

void
MainWindow::on_yawControlSpinBox_valueChanged(double arg1)
{
  if (ui->yawControlSlider->value() != (int)arg1)
  {
    ui->yawControlSlider->setValue((int)arg1);
  }
  //this->command.yaw = arg1;
}

void
MainWindow::on_yawControlSlider_valueChanged(int value)
{
  ui->yawControlSpinBox->setValue(value);
}

void
MainWindow::handleXYRanges(double minXY, double maxXY)
{
  //! X min
  ui->xControlSlider->setMinimum(minXY);
  ui->xControlSpinBox->setMinimum(minXY);
  ui->xControlMin->setText(QString::number(minXY));

  //! X Max
  ui->xControlSlider->setMaximum(maxXY);
  ui->xControlSpinBox->setMaximum(maxXY);
  ui->xControlMax->setText(QString::number(maxXY));

  //! X default value
  ui->xControlSlider->setValue(0);

  //! Y Min
  ui->yControlSlider->setMinimum(minXY);
  ui->yControlSpinBox->setMinimum(minXY);
  ui->yControlMin->setText(QString::number(minXY));

  //! Y Max
  ui->yControlSlider->setMaximum(maxXY);
  ui->yControlSpinBox->setMaximum(maxXY);
  ui->yControlMax->setText(QString::number(maxXY));

  //! Y default value
  ui->yControlSlider->setValue(0);
}

void
MainWindow::handleZRanges(double minZ, double maxZ)
{
  //! Z min
  ui->zControlSlider->setMinimum(minZ);
  ui->zControlSpinBox->setMinimum(minZ);
  ui->zControlMin->setText(QString::number(minZ));

  //! Z max
  ui->zControlSlider->setMaximum(maxZ);
  ui->zControlSpinBox->setMaximum(maxZ);
  ui->zControlMax->setText(QString::number(maxZ));

  //! Z default value
  ui->zControlSlider->setValue(0);
}

void
MainWindow::handleYawRanges(double minYaw, double maxYaw)
{
  //! Yaw max
  ui->yawControlSlider->setMinimum(minYaw);
  ui->yawControlSpinBox->setMinimum(minYaw);
  ui->yawControlMin->setText(QString::number(minYaw));

  //! Yaw min
  ui->yawControlSlider->setMaximum(maxYaw);
  ui->yawControlSpinBox->setMaximum(maxYaw);
  ui->yawControlMin->setText(QString::number(maxYaw));

  //! Yaw default value
  ui->yawControlSlider->setValue(0);
}


void MainWindow::flightSend()
{
     QString Flights;
     QByteArray data2;
     QString data1=QString("%1").arg(ui->xControlSpinBox->value());
      Flights="Flight!";
      Flights+=data1+"!";
      data1=QString("%1").arg(ui->yControlSpinBox->value());
      Flights+=data1+"!";
      data1=QString("%1").arg(ui->zControlSpinBox->value());
      Flights+=data1+"!";
      data1=QString("%1").arg(ui->yawControlSpinBox->value());
      Flights+=data1+"!";
      //
      Flights+=flight_data+"!\r\n";

      data2=Flights.toLatin1();
      serial->write(tr(data2).toLatin1());

      // Flights :  "Flight!x!y!z!yaw!horizontal!vertical!YawType!Frame!Stable!\r\n"
      // Flights index:   0!1!2!3!4  !5         !6       !7      !8    !9     ! 10

}



void MainWindow::on_Flightstart_clicked()
{
    // 按下就非使能
    ui->Flightstart->setDisabled(true);
    serial->write("start!!!!!!!!!!\r\n");
    point_count = 0;
    /*
    if(ui->Flightstart->isEnabled())
        qDebug() << "Yes";
    else
        qDebug() << "No";
    */
}

void MainWindow::on_Ending_clicked()
{
    if(ui->Flightstart->isEnabled()==false)
    {
        ui->Flightstart->setDisabled(false);
    }
    serial->write("ending"+QString("%1").arg(point_count,2,10,QLatin1Char('0')).toLatin1() + "!!!!!!!\r\n");
    qDebug() << "ending!!!!!!!!!";
    point_count = 0;
}




void MainWindow::on_Flying_clicked()
{
    serial->write("flying!!!!!!!!!\r\n");
    qDebug() << "flying!!!!!!!!!";
}

void MainWindow::on_GoHome_clicked()
{

    //serial->write("GoHome!!!!!!!!!\r\n");

    serial->write("start!!!!!!!!!!\r\n");
    serial->write("la00:0.4042677!\r\n");
    serial->write("lo00:1.9781439!\r\n");

    serial->write("la01:0.4042588!\r\n");
    serial->write("lo01:1.9781436!\r\n");


    serial->write("la02:0.4042581!\r\n");
    serial->write("lo02:1.9781524!\r\n");


    serial->write("la03:0.4042667!\r\n");
    serial->write("lo03:1.9781526!\r\n");


    serial->write("ending04!!!!!!!\r\n");

    serial->write("flying!!!!!!!!!\r\n");

}
