//    Distributed under MIT License.
//    Author: Dr. Ing. Diego D. Santiago
//    Institution: INAUT - UNSJ - CONICET
//    Email: dsantiago@inaut.unsj.edu.ar


#include "stepperparameters.h"
#include "ui_stepperparameters.h"
#include <qdebug.h>


StepperParameters::StepperParameters(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StepperParameters)
{
    ui->setupUi(this);
    updateParams();

}

StepperParameters::~StepperParameters()
{
    delete ui;
}

void StepperParameters::on_rb_dirLeft_toggled(bool checked)
{
    stepper_params.rot_dir=uint8_t(checked);
    emit parameterChanged();
}

void StepperParameters::on_cb_msi1_toggled(bool checked)
{
    ms1_val=checked;
    stepper_params.step_res=getStepMultiplicator(ms1_val,ms2_val,ms3_val);
    emit parameterChanged();

}

void StepperParameters::on_cb_msi2_toggled(bool checked)
{
    ms2_val=checked;
    stepper_params.step_res=getStepMultiplicator(ms1_val,ms2_val,ms3_val);
    emit parameterChanged();

}

void StepperParameters::on_cb_msi3_toggled(bool checked)
{
    ms3_val=checked;
    stepper_params.step_res=getStepMultiplicator(ms1_val,ms2_val,ms3_val);
    emit parameterChanged();

}

uint8_t  StepperParameters::getStepMultiplicator(bool ms1,bool ms2,bool ms3){
    if(!(ms1||ms2||ms3))
        return 0;
    if(ms1 && !(ms2||ms3))
        return 1;
    if(ms2 && !(ms1||ms3))
        return 2;
    if( ms1 && ms2 && !ms3)
        return 3;
    if(ms1 && ms2 && ms3)
        return 4;
    return 0;
}



void StepperParameters::on_hs_freq_valueChanged(int value)
{
    ui->le_freq->setText(QString::number(value));
    stepper_params.freq=value;
    emit parameterChanged();
}

void StepperParameters::on_hs_duration_valueChanged(int value)
{
    ui->le_duration->setText(QString::number(value));
    stepper_params.pulse_duration=value;
    emit parameterChanged();
}

void StepperParameters::on_le_freq_editingFinished()
{
    QString arg1=ui->le_freq->text();
    ui->hs_freq->setValue(arg1.toInt());
    stepper_params.freq=arg1.toUShort();
    emit parameterChanged();
}

void StepperParameters::on_le_duration_editingFinished()
{
    QString arg1=ui->le_duration->text();
    ui->hs_duration->setValue(arg1.toInt());
    if(arg1!="inf")
    {
        stepper_params.pulse_duration=arg1.toUInt();
    }
    else {
        stepper_params.pulse_duration=0;
    }
    emit parameterChanged();
}

void StepperParameters::updateParams(){
    ms1_val=ui->cb_msi1->isChecked();
    ms2_val=ui->cb_msi2->isChecked();
    ms3_val=ui->cb_msi3->isChecked();
//    qDebug()<<ms1_val;
//    qDebug()<<ms2_val;
//    qDebug()<<ms3_val;


    stepper_params.rot_dir             = ui->rb_dirLeft->isChecked(); //rotation direction (left/rigth)
    stepper_params.step_res            = getStepMultiplicator(ms1_val,ms2_val,ms3_val); //step resolution (0=1; 1=1/2; 2=1/4; 3=1/8; 4=1/16)
    stepper_params.freq                = ui->hs_freq->value();;      //pulse signal frequency
    stepper_params.pulse_duration      = ui->hs_duration->value();; //pulse signal duration

    ui->le_freq->setText(QString::number(stepper_params.freq ) );
    ui->le_duration->setText(QString::number(stepper_params.pulse_duration ) );
}

void StepperParameters::on_le_duration_textChanged(const QString &arg1)
{
    ui->hs_duration->setValue(arg1.toInt());
}

void StepperParameters::on_le_freq_textChanged(const QString &arg1)
{
    ui->hs_freq->setValue(arg1.toInt());
}
