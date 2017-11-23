#include "plot1d.h"
#include <QtCharts/QLineSeries>
#include <QPen>
#include <complex>
#include <QDebug>
#include <QtCore/qmath.h>

using namespace std;
using namespace QtCharts;

Plot1D::Plot1D(QWidget *parent) : QChartView(parent),
  _chart(new QChart()),
  _series_real(new QLineSeries()),
  _series_imag(new QLineSeries()),
  _series_magn(new QLineSeries()),
  _x_min(0),
  _x_max(1)
{
    _chart->setBackgroundBrush(Qt::black);
    _series_real->setPen(QPen(QColor::fromRgb(255, 255, 127), 2, Qt::SolidLine));
    _series_imag->setPen(QPen(QColor::fromRgb(255, 255, 255), 2, Qt::SolidLine));
    _series_magn->setPen(QPen(QColor::fromRgb(215, 115, 115), 2, Qt::SolidLine));

    _chart->legend()->hide();
    setChart(_chart);
}

bool Plot1D::SetData(const std::vector<float> &data)
{
    Clear();

    for(unsigned int i = 0; i < data.size(); ++i)
    {
//        double I = double(_x_min) + double(i) * (double(_x_max - _x_min) / double(data.size()));
//        _series_magn->append(I, data[i]);
        _series_magn->append(i, data[i]);
    }

    _chart->addSeries(_series_magn);

    _chart->createDefaultAxes();
    _chart->axisX(_series_magn)->setGridLineColor(Qt::gray);
    _chart->axisY(_series_magn)->setGridLineColor(Qt::gray);
    _chart->axisX(_series_magn)->setLabelsColor(Qt::white);
    _chart->axisY(_series_magn)->setLabelsColor(Qt::white);

    update();

    return true;
}

bool Plot1D::SetData(const std::vector<std::complex<float> > &data)
{
    Clear();

    for(unsigned int i = 0; i < data.size(); ++i)
    {
//        double I = double(_x_min) + double(i) * (double(_x_max - _x_min) / double(data.size()));
//        _series_real->append(I, data[i].real());
//        _series_imag->append(I, data[i].imag());
//        auto amp = sqrt(data[i].real() * data[i].real() + data[i].imag() * data[i].imag());
//        _series_magn->append(I, amp);

        _series_real->append(i, data[i].real());
        _series_imag->append(i, data[i].imag());
        auto amp = sqrt(data[i].real() * data[i].real() + data[i].imag() * data[i].imag());
        _series_magn->append(i, amp);
    }

    _chart->addSeries(_series_real);
    _chart->addSeries(_series_imag);
    _chart->addSeries(_series_magn);

    _chart->createDefaultAxes();
    _chart->axisX(_series_magn)->setGridLineColor(Qt::gray);
    _chart->axisY(_series_magn)->setGridLineColor(Qt::gray);
    _chart->axisX(_series_magn)->setLabelsColor(Qt::white);
    _chart->axisY(_series_magn)->setLabelsColor(Qt::white);

    update();

    return true;
}

bool Plot1D::SetAxisX(int min, int max)
{
    _x_min = min;
    _x_max = max;
    return true;
}

void Plot1D::Clear()
{    
    _series_imag->clear();
    _series_magn->clear();
    _series_real->clear();

    _chart->removeSeries(_series_imag);
    _chart->removeSeries(_series_magn);
    _chart->removeSeries(_series_real);
}

