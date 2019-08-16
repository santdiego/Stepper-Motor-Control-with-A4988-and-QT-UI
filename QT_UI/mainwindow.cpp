//    Distributed under MIT License.
//    Author: Dr. Ing. Diego D. Santiago
//    Institution: INAUT - UNSJ - CONICET
//    Email: dsantiago@inaut.unsj.edu.ar

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QLabel>
#include <QMessageBox>
#include <QtDebug>
//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_status(new QLabel),
    m_settings(new SettingsDialog),
    m_stepper(new StepperParameters),
    m_serial(new QSerialPort(this))
//! [1]
{
    //! [0]
    m_ui->setupUi(this);
    m_ui->mainLayout->addWidget(m_stepper);


    //centralWidget()->setLayout();

    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);

    m_ui->statusBar->addWidget(m_status);

    initActionsConnections();

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    //connect(&timer, &QTimer::timeout, this, &MainWindow::writeInstructions);
    //connect(m_stepper, &StepperParameters::parameterChanged, this, &MainWindow::writeInstructions);
    connect(m_ui->actionPlay, &QAction::triggered, m_stepper, &StepperParameters::updateParams);
    connect(m_ui->actionPlay, &QAction::triggered, this, &MainWindow::writeInstructions);

}


MainWindow::~MainWindow()
{

    delete m_settings;
    delete m_ui;
}

//! [4]
void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {

        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                              .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                              .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));

    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}
//! [4]
void MainWindow::run(){
    const SettingsDialog::Settings p = m_settings->settings();
    if(m_serial->isOpen())
    {
        //m_serial->clear();
        if(p.writeRate_ms.toInt()>0)
            timer.start(p.writeRate_ms.toInt());
        else {
            writeInstructions();
        }
    }
}
//! [5]
void MainWindow::closeSerialPort()
{
    timer.stop();
    if (m_serial->isOpen())
        m_serial->close();
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}
//! [5]

void MainWindow::about()
{
    QMessageBox::about(this, tr("Motor PID Tuning"),
                       tr("Program that allows to configure the PID parameters for DC Motor Velocity Control."
                          "\n Author: Dr. Ing. Diego D. Santiago. "));
}

//! [6]
void MainWindow::writeData(const QByteArray &data)
{
    m_serial->write(data);
}

void MainWindow::writeInstructions()
{


    uint8_t command[10];//2byte Addr +1byte rot_dir +1byte step_res+2byte freq+4byte pulse_duration;
    byte16 tmp;
    tmp.value=uint16_t(0);//addr
    //1º Address
    command[0]=tmp.word[0];
    command[1]=tmp.word[1];
    //2º Rotation Direction
    qDebug()<<"rot_dir="<<m_stepper->stepper_params.rot_dir;
    command[2]=m_stepper->stepper_params.rot_dir;

    //3º Step resolution
    command[3]=m_stepper->stepper_params.step_res;
   qDebug()<<"step_res="<<m_stepper->stepper_params.step_res;
    //4º Step signal Frequency
    union{
        byte byte[2];
        uint16_t value;
    }aux;
    qDebug()<<"freq="<<m_stepper->stepper_params.freq;
    aux.value=m_stepper->stepper_params.freq;
    command[4]=aux.byte[0];
    command[5]=aux.byte[1];


    //5º pulse duration
    union{
        byte byte[4];
        unsigned value;
    }aux2;
    aux2.value=m_stepper->stepper_params.pulse_duration;
    qDebug()<<"pulse_duration="<<m_stepper->stepper_params.pulse_duration;
    command[6]=aux2.byte[0];
    command[7]=aux2.byte[1];
    command[8]=aux2.byte[2];
    command[9]=aux2.byte[3];

    instructionFrame.buildInstructionPacket(1,Write,command,sizeof(command));
    byte temp[BUFFERSIZE];
    uint16_t size;
    instructionFrame.getLastFrame(temp,size);
//    for (int i=0;i<size;i++) {
//        qDebug()<<hex<<temp[i];
//    }
    m_serial->write(reinterpret_cast<char*>(temp),int(size));

}
//! [6]

//! [7]
void MainWindow::readData()
{
    const QByteArray data = m_serial->readAll();
    qDebug()<<data;
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}
//! [8]

void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(m_ui->actionPlay, &QAction::triggered, this, &MainWindow::run);
    connect(m_ui->actionStop, &QAction::triggered, this, &MainWindow::stop);

}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

void MainWindow::stop(){
    timer.stop();
}
