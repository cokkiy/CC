/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "packetgraph.h"
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dtheme.h>
#include <QtDataVisualization/qbar3dseries.h>
#include <QtDataVisualization/qcategory3daxis.h>
#include <QtDataVisualization/qvalue3daxis.h>
#include <QtGui/QFont>
#include <QtGui/QGuiApplication>

using namespace QtDataVisualization;

PacketGraph::PacketGraph(Q3DBars *rainfall) : m_graph(rainfall) {

  // prepare data
  pretreatingData();

  m_columnCount = m_protos.size();
  m_rowCount = m_sources.size();

  m_proxy = new VariantBarDataProxy;
  QBar3DSeries *series = new QBar3DSeries(m_proxy);
  m_graph->addSeries(series);

  // Set up bar specifications; make the bars as wide as they are deep,
  // and add a small space between the bars
  m_graph->setBarThickness(1.0f);
  m_graph->setBarSpacing(QSizeF(1.1, 1.1));

  m_graph->rowAxis()->setTitle(QStringLiteral("源"));
  m_graph->columnAxis()->setTitle(QStringLiteral("协议"));
  m_graph->valueAxis()->setTitle(QStringLiteral("字节"));
  m_graph->valueAxis()->setLabelFormat("%d bytes");
  m_graph->valueAxis()->setSegmentCount(5);
  m_graph->rowAxis()->setLabels(m_sources.toList());
  m_graph->columnAxis()->setLabels(m_protos.toList());


  // Set bar type to cylinder
  series->setMesh(QAbstract3DSeries::MeshCylinder);

  // Set shadows to medium
  m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualityMedium);

  // Set selection mode to bar and column
  m_graph->setSelectionMode(QAbstract3DGraph::SelectionItemAndColumn |
                            QAbstract3DGraph::SelectionSlice);

  // Set theme
  m_graph->activeTheme()->setType(Q3DTheme::ThemePrimaryColors);

  // Override font in theme
  m_graph->activeTheme()->setFont(QFont("Century Gothic", 30));

  // Override label background for theme
  m_graph->activeTheme()->setLabelBackgroundEnabled(false);

  // Set camera position and zoom
  m_graph->scene()->activeCamera()->setCameraPreset(
      Q3DCamera::CameraPresetIsometricRightHigh);

  m_graph->setCursor(Qt::PointingHandCursor);

  // Set reflections on
  m_graph->setReflection(true);
}

PacketGraph::~PacketGraph() {
  delete m_mapping;
  delete m_dataSet;
  delete m_graph;
}

void PacketGraph::start() { addDataSet(); }



void PacketGraph::pretreatingData(){
  QTextStream stream;
  QFile dataFile(":/data/data.txt");
  if (dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    stream.setDevice(&dataFile);
    while (!stream.atEnd()) {
      QString line = stream.readLine();
      if (line.startsWith("#")) // Ignore comments
        continue;
      QStringList strList = line.split(",", QString::SkipEmptyParts);
      // Each line has three data items: Year, month, and rainfall value
      if (strList.size() < 6) {
        qWarning() << "Invalid row read from data:" << line;
        continue;
      }
      QString src = strList.at(2);
      src.remove("\"");
      if (!m_sources.contains(src.trimmed())) {
        m_sources.insert(src.trimmed());
      }
      QString proto = strList.at(4);
      proto.remove("\"");
      if (!m_protos.contains(proto.trimmed())) {
        m_protos.insert(proto.trimmed());
      }

      QString key = QString("%1-%2").arg(src).arg(proto);
      QString countStr = strList.at(5);
      countStr.remove(QString("\""));
      double v = countStr.trimmed().toDouble();
      m_counts[key] = m_counts.value(key, 0) + v;
    }
  } else {
    qWarning() << "Unable to open data file:" << dataFile.fileName();
  }
}

//! [0]
void PacketGraph::addDataSet() {
  // Create a new variant data set and data item list
  m_dataSet = new VariantDataSet;
  VariantDataItemList *itemList = new VariantDataItemList;

  for(QString src:m_sources)
  {    
    for(QString proto:m_protos)
    {
        VariantDataItem *newItem = new VariantDataItem;
        newItem->append(src);
        newItem->append(proto);
        QString key=QString("%1-%2").arg(src).arg(proto);
        newItem->append(m_counts.value(key,0));
        itemList->append(newItem);
    }    
  }

  //! [1]
  // Add items to the data set and set it to the proxy
  m_dataSet->addItems(itemList);
  m_proxy->setDataSet(m_dataSet);

  // Create new mapping for the data and set it to the proxy
  m_mapping = new VariantBarDataMapping(0, 1, 2, m_sources.toList(), m_protos.toList());
  m_proxy->setMapping(m_mapping);
  //! [1]
}
//! [0]
