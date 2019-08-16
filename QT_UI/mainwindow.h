//    Distributed under MIT License.
//    Author: Dr. Ing. Diego D. Santiago
//    Institution: INAUT - UNSJ - CONICET
//    Email: dsantiago@inaut.unsj.edu.ar

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSharedMemory>
#include <QTimer>
#include "serialframedecoder.h"
#include "stepperparameters.h"

#define SHM_MOTOR_IN    "motor_in"
#define SHM_MOTOR_OUT   "motor_out"


QT_BEGIN_NAMESPACE

class QLabel;



namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void writeInstructions();
    void readData();
    void run();
    void handleError(QSerialPort::SerialPortError error);

    void stop();
private:
    void initActionsConnections();
    void showStatusMessage(const QString &message);

    Ui::MainWindow *m_ui = nullptr;
    QLabel *m_status = nullptr;
    SettingsDialog *m_settings = nullptr;
    StepperParameters *m_stepper = nullptr;

    QSerialPort *m_serial = nullptr;
    SerialFrameDecoder frameDecode;
    SerialFrameDecoder instructionFrame;
    QTimer timer;

};

#endif // MAINWINDOW_H
