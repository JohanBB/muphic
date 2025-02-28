#ifndef MUPHICCONFIG_H
#define MUPHICCONFIG_H

#include <QTabWidget>
#include "UsrConf.h"
#include "ui_muphicconfig.h"
#include "launcher.h"
#include "polygonwidget.h"
#include "UsrConf.h"
#include "aux_functions.h"

#include <QFile>
#include <QMessageBox>
#include <QFileDialog>

#include <QDesktopServices>
#include <QUrl>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPixmap>
#include <QTime>
#include <QSize>


#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <phonon/mediasource.h>

/*
#include <Phonon/AudioOutput>
#include <Phonon/SeekSlider>
#include <Phonon/MediaObject>
#include <Phonon/VolumeSlider>
#include <Phonon/BackendCapabilities>
#include <Phonon/MediaSource>
*/

#define DEFAULT_PIC "./test1.png"

namespace Ui {
class MuphicConfig;
}

class MuphicConfig : public QTabWidget
{
    Q_OBJECT
    
public:
    UsrConf * usrConf;

    explicit MuphicConfig(QWidget *parent = 0);
    ~MuphicConfig();
    void initialize();
    void destroy();

    void paintEvent(QPaintEvent*);

private slots:

    void on_filterSelComboBox_currentIndexChanged(int index);

    void on_horizontalSlider_2_valueChanged(int position);

    void on_horizontalSlider_6_valueChanged(int position);

    void on_horizontalSlider_colorLevels_valueChanged(int position);

    void on_horizontalSliderTS_valueChanged(int position);

    void on_horizontalSliderTH_valueChanged(int position);

    void on_horizontalSliderTV_valueChanged(int position);

    void on_comboBox_reconColors_currentIndexChanged(int index);

    void on_comboBox_v1Composer_currentIndexChanged(int index);

    void on_comboBox_v1Instrument_currentIndexChanged(int index);

    void on_comboBox_v2Composer_currentIndexChanged(int index);

    void on_comboBox_v2Instruments_currentIndexChanged(int index);

    void on_comboBox_v3Composer_currentIndexChanged(int index);

    void on_comboBox_v3Instrument_currentIndexChanged(int index);

    void on_comboBox_v4Composer_currentIndexChanged(int index);

    void on_comboBox_v4Instrument_currentIndexChanged(int index);

    void on_comboBox_composerMixer_currentIndexChanged(int index);

    void on_comboBox_v3Composer_3_currentIndexChanged(int index);

    void on_horizontalSlider_3_valueChanged(int value);

    void on_horizontalSlider_AnalisisDepth_valueChanged(int value);

    void on_toolButton_OutputMidi_clicked();

    void on_toolButton_InputPic_clicked();

    void on_pushButton_Generate_clicked();

    void on_pushButton_Stop_clicked();

    void on_pushButton_Play_clicked();

    void on_pushButton_Analyze_clicked();

    void stateChanged(Phonon::State newState, Phonon::State oldState);

    void tick(qint64 time);

    void setupActions();

    void on_pushButton_pause_clicked();

    void on_comboBox_Tempo_currentIndexChanged(int index);

    void on_pushButton_score_clicked();

private:
    Ui::MuphicConfig *ui;

    QSize label_2_max;
    QSize tlabel_max;
    QSize hSlider_2_max;
    QSize labelColorLevels_max;
    QSize label_colorLevels_max;
    QSize hSliderColorLevels_max;
    QSize labelThresholdH_max;
    QSize label_ThresholdH_max;
    QSize hSliderThersholdH_max;
    QSize labelThresholdS_max;
    QSize label_ThresholdS_max;
    QSize hSliderThersholdS_max;
    QSize labelThresholdV_max;
    QSize label_ThresholdV_max;
    QSize hSliderThersholdV_max;

    QGraphicsScene * newScene;
    PolygonWidget * poly;

    QString imageFile;
    QString outputFile;
    Launcher* l;
    pid_t pidAnal, pidComp;

    bool analysing;
    bool composing;


    Phonon::MediaObject *mediaObject;
    Phonon::AudioOutput *audioOutput;
    Phonon::MediaSource mediaSource;

    bool is_paused;
    qint64 mediaTime;

    QAction *playAction;
    QAction *pauseAction;
    QAction *stopAction;
};

#endif // MUPHICCONFIG_H
