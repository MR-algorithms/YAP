#ifndef PLOT1D_H
#define PLOT1D_H

#include <qtcharts/qchartview.h>
#include <QtCharts/QLineSeries>

#include <vector>
#include <complex>

class Plot1D : public QtCharts::QChartView
{
    Q_OBJECT
public:
    explicit Plot1D(QWidget *parent = 0);
    bool SetData(const std::vector<float>& data);
    bool SetData(const std::vector<std::complex<float>>& data);

    bool SetAxisX(int min, int max);

signals:

public slots:

private:
    QtCharts::QChart * _chart;
    QtCharts::QLineSeries * _series_real;
    QtCharts::QLineSeries * _series_imag;
    QtCharts::QLineSeries * _series_magn;

    int _x_min;
    int _x_max;

    void Clear();
};

#endif // PLOT1D_H
