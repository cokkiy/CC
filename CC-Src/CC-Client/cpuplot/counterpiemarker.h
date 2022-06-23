#pragma once

//-----------------------------------------------------------------
// This class shows how to extend QwtPlotItems. It displays a
// pie chart of user/total/idle cpu usage in percent.
//-----------------------------------------------------------------

#include <qwt_plot_item.h>
struct PerfCurves;
class CounterPieMarker : public QwtPlotItem {
public:
  CounterPieMarker(PerfCurves *perfCurves, int size, bool isCPUPlot = true);

  virtual int rtti() const;

  virtual void draw(QPainter *, const QwtScaleMap &, const QwtScaleMap &,
                    const QRectF &) const;

private:
  PerfCurves *perfCurves;
  int size;
  bool isCPUPlot;
};
