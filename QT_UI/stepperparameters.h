//    Distributed under MIT License.
//    Author: Dr. Ing. Diego D. Santiago
//    Institution: INAUT - UNSJ - CONICET
//    Email: dsantiago@inaut.unsj.edu.ar

#ifndef STEPPERPARAMETERS_H
#define STEPPERPARAMETERS_H

#include <QWidget>


struct Stepper_Control_Params{
    uint8_t       rot_dir             = 0; //rotation direction (left/rigth)
    uint8_t       step_res            = 0; //step resolution (0=1; 1=1/2; 2=1/4; 3=1/8; 4=1/16)
    uint16_t      freq                = 1;      //pulse signal frequency
    unsigned long pulse_duration      = 0; //pulse signal duration
    Stepper_Control_Params(){
        memset(this,0,sizeof (Stepper_Control_Params));
    }
};

namespace Ui {
class StepperParameters;
}

class StepperParameters : public QWidget
{
    Q_OBJECT

public:
    explicit StepperParameters(QWidget *parent = nullptr);
    ~StepperParameters();
    Stepper_Control_Params stepper_params;
public slots:
    void updateParams();
private slots:
    void on_rb_dirLeft_toggled(bool checked);

    void on_cb_msi1_toggled(bool checked);

    void on_cb_msi2_toggled(bool checked);

    void on_cb_msi3_toggled(bool checked);

    void on_hs_freq_valueChanged(int value);

    void on_hs_duration_valueChanged(int value);

    void on_le_duration_editingFinished();

    void on_le_freq_editingFinished();

    void on_le_duration_textChanged(const QString &arg1);

    void on_le_freq_textChanged(const QString &arg1);

signals:
    void parameterChanged();
private:
    Ui::StepperParameters *ui;
    bool ms1_val;
    bool ms2_val;
    bool ms3_val;
    uint8_t getStepMultiplicator(bool ms1, bool ms2, bool ms3);
};

#endif // STEPPERPARAMETERS_H
