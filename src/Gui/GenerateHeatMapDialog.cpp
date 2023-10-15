
/*
 * Copyright (c) 2011 Mark Liversedge (liversedge@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "GenerateHeatMapDialog.h"
#include "MainWindow.h"
#include "Context.h"
#include "Athlete.h"
#include "RideCache.h"
#include "Colors.h"
#include "SearchFilterBox.h"
#include "HelpWhatsThis.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QListIterator>
#include <QDebug>

GenerateHeatMapDialog::GenerateHeatMapDialog(Context *context, const QString &filter) : QDialog(context->mainWindow), context(context)
{
    setAttribute(Qt::WA_DeleteOnClose);
    //setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint); // must stop using this flag!
    setWindowTitle(tr("Heat Map Generator"));
    HelpWhatsThis *help = new HelpWhatsThis(this);
    this->setWhatsThis(help->getWhatsThisText(HelpWhatsThis::MenuBar_Tools_CreateHeatMap));

    // make the dialog a resonable size
    setMinimumWidth(550 *dpiXFactor);
    setMinimumHeight(400 *dpiYFactor);

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    files = new QTreeWidget;
    files->headerItem()->setText(0, tr(""));
    files->headerItem()->setText(1, tr("Filename"));
    files->headerItem()->setText(2, tr("Date"));
    files->headerItem()->setText(3, tr("Time"));
    files->headerItem()->setText(4, tr("Action"));

    files->setColumnCount(5);
    files->setColumnWidth(0, 30 *dpiXFactor); // selector
    files->setColumnWidth(1, 190 *dpiXFactor); // filename
    files->setColumnWidth(2, 95 *dpiXFactor); // date
    files->setColumnWidth(3, 90 *dpiXFactor); // time
    files->setSelectionMode(QAbstractItemView::SingleSelection);
    files->setEditTriggers(QAbstractItemView::SelectedClicked); // allow edit
    files->setUniformRowHeights(true);
    files->setIndentation(0);

    // populate with each ride in the ridelist
    QStringList allowList = SearchFilterBox::matches(context, filter);
    foreach (RideItem *rideItem, context->athlete->rideCache->rides()) {
        if (! allowList.contains(rideItem->fileName)) {
            // Current RideItem is hidden by filter
            continue;
        }

        QTreeWidgetItem *add = new QTreeWidgetItem(files->invisibleRootItem());
        add->setFlags(add->flags() | Qt::ItemIsEditable);

        // selector
        QCheckBox *checkBox = new QCheckBox("", this);
        checkBox->setChecked(true);
        files->setItemWidget(add, 0, checkBox);

        // we will wipe the original file
        add->setText(1, rideItem->fileName);
        add->setText(2, rideItem->dateTime.toString(tr("dd MMM yyyy")));
        add->setText(3, rideItem->dateTime.toString("hh:mm:ss"));

        // interval action
        add->setText(4, tr("Export"));
    }

    // directory
    QGridLayout *configGrid = new QGridLayout;
    int row = 0;

    // default to last used
    QString dirDefault = appsettings->value(this, GC_BE_LASTDIR, QDir::home().absolutePath()).toString();
    if (!QDir(dirDefault).exists()) dirDefault = QDir::home().absolutePath();

    selectDir = new QPushButton(tr("Browse"), this);
    dirName = new QLabel(dirDefault, this);
    dirLabel = new QLabel (tr("Export to"), this);

    mapProvider = new QComboBox(this);
    mapProvider->addItem("OSM");
    mapProvider->addItem("Google");

    osmURLLabel = new QLabel(tr("Custom OSM URL"));
    osmURLInput = new QLineEdit(this);
    osmURLInput->setPlaceholderText("Enter optional OSM URL (without xyz)");

    googleAPIKeyLabel = new QLabel(tr("Google API key"));
    googleAPIKeyInput = new QLineEdit(this);

    configGrid->addWidget(dirLabel, row, 0, Qt::AlignLeft);
    configGrid->addWidget(dirName, row, 1, Qt::AlignLeft);
    configGrid->addWidget(selectDir, row, 2, Qt::AlignLeft);
    ++row;
    configGrid->addWidget(new QLabel(tr("Map Provider")), row, 0, Qt::AlignLeft);
    configGrid->addWidget(mapProvider, row, 1, 1, 2, Qt::AlignLeft);
    ++row;
    configGrid->addWidget(osmURLLabel, row, 0, Qt::AlignLeft);
    configGrid->addWidget(osmURLInput, row, 1, 1, 2, Qt::AlignLeft);
    ++row;
    configGrid->addWidget(googleAPIKeyLabel, row, 0, Qt::AlignLeft);
    configGrid->addWidget(googleAPIKeyInput, row, 1, 1, 2, Qt::AlignLeft);
    ++row;
    configGrid->setColumnStretch(0, 1);
    configGrid->setColumnStretch(1, 10);

    configFrame = new QFrame(this);
    configFrame->setFrameStyle(QFrame::StyledPanel);
    configFrame->setLineWidth(2);
    configFrame->setLayout(configGrid);

    mapProviderSelected(mapProvider->currentIndex());

    // Fileframe
    QLabel *filterLabel;
    if (! SearchFilterBox::isNull(filter)) {
        filterLabel = new QLabel(QString(filter), this);
    } else {
        filterLabel = new QLabel(QString(tr("<i>no filter</i>")), this);
    }

    all = new QCheckBox(tr("check/uncheck all"), this);
    all->setChecked(true);

    QGridLayout *fileGrid = new QGridLayout();
    row = 0;

    fileGrid->addWidget(new QLabel(tr("Active Filter")), row, 0, Qt::AlignLeft);
    fileGrid->addWidget(filterLabel, row, 1, Qt::AlignLeft);
    ++row;
    fileGrid->addWidget(all, row, 0, Qt::AlignLeft);
    ++row;
    fileGrid->addWidget(files, row, 0, 1, 2);
    ++row;
    fileGrid->setColumnStretch(0, 1);
    fileGrid->setColumnStretch(1, 10);

    fileFrame = new QFrame(this);
    fileFrame->setFrameStyle(QFrame::StyledPanel);
    fileFrame->setLineWidth(2);
    fileFrame->setLayout(fileGrid);

    // buttons
    QHBoxLayout *buttons = new QHBoxLayout;
    status = new QLabel("", this);
    status->hide();
    cancel = new QPushButton(tr("Cancel"), this);
    ok = new QPushButton(tr("Generate Heat Map"), this);
    buttons->addWidget(status);
    buttons->addStretch();
    buttons->addWidget(cancel);
    buttons->addWidget(ok);

    layout->addWidget(configFrame);
    layout->addWidget(fileFrame);

    layout->addSpacing(10 * dpiYFactor);
    layout->addLayout(buttons);

    exports = fails = 0;

    // connect signals and slots up..
    connect(selectDir, SIGNAL(clicked()), this, SLOT(selectClicked()));
    connect(mapProvider, SIGNAL(currentIndexChanged(int)), this, SLOT(mapProviderSelected(int)));
    connect(ok, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(all, SIGNAL(stateChanged(int)), this, SLOT(allClicked()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancelClicked()));
}

void
GenerateHeatMapDialog::selectClicked()
{
    QString before = dirName->text();
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Target Directory"),
                                                 before,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (dir!="") {
        dirName->setText(dir);
    }
    return;
}

void
GenerateHeatMapDialog::allClicked()
{
    // set/uncheck all rides according to the "all"
    bool checked = all->isChecked();

    for(int i=0; i<files->invisibleRootItem()->childCount(); i++) {
        QTreeWidgetItem *current = files->invisibleRootItem()->child(i);
        static_cast<QCheckBox*>(files->itemWidget(current,0))->setChecked(checked);
    }
}


void
GenerateHeatMapDialog::mapProviderSelected
(int index)
{
    bool osm = (index == 0);
    osmURLLabel->setVisible(osm);
    osmURLInput->setVisible(osm);
    googleAPIKeyLabel->setVisible(! osm);
    googleAPIKeyInput->setVisible(! osm);
}

void
GenerateHeatMapDialog::okClicked()
{
    configFrame->setEnabled(false);
    fileFrame->setEnabled(false);
    if (ok->text() == "Generate Heat Map" || ok->text() == tr("Generate Heat Map")) {
        aborted = false;

        status->setText(tr("Generating Heat Map..."));
        status->show();
        cancel->hide();
        ok->setText(tr("Abort"));
        appsettings->setValue(GC_BE_LASTDIR, dirName->text());
        generateNow();
        status->setText(QString(tr("%1 activities exported, %2 failed or skipped.")).arg(exports).arg(fails));
        ok->setText(tr("Finish"));

    } else if (ok->text() == "Abort" || ok->text() == tr("Abort")) {
        aborted = true;
    } else if (ok->text() == "Finish" || ok->text() == tr("Finish")) {
        accept(); // our work is done!
    }
}

void
GenerateHeatMapDialog::cancelClicked()
{
    reject();
}

void
GenerateHeatMapDialog::generateNow()
{
    double minLat = 999;
    double maxLat = -999;
    double minLon = 999;
    double maxLon = -999;
    QHash<QString, int> hash;
    int maxCount = 0;

    // loop through the table and export all selected
    for(int i=0; i<files->invisibleRootItem()->childCount(); i++) {

        // give user a chance to abort..
        QApplication::processEvents();

        // did they?
        if (aborted == true) return; // user aborted!

        QTreeWidgetItem *current = files->invisibleRootItem()->child(i);

        // is it selected
        if (static_cast<QCheckBox*>(files->itemWidget(current,0))->isChecked()) {

            files->setCurrentItem(current); QApplication::processEvents();

            // this one then
            current->setText(4, tr("Reading...")); QApplication::processEvents();

            // open it..
            QStringList errors;
            QList<RideFile*> rides;
            QFile thisfile(QString(context->athlete->home->activities().absolutePath()+"/"+current->text(1)));
            RideFile *ride = RideFileFactory::instance().openRideFile(context, thisfile, errors, &rides);

            // open success?
            if (ride) {
                current->setText(4, tr("Writing...")); QApplication::processEvents();

                if (ride->areDataPresent()->lat == true && ride->areDataPresent()->lon == true) {
                    int lastDistance = 0;
                    foreach(const RideFilePoint *point, ride->dataPoints()) {

                        if (lastDistance < (int) (point->km * 1000) &&
                           (point->lon!=0 || point->lat!=0)) {

                            // Pick up a point max every 15m
                            lastDistance = (int) (point->km * 1000) + 15;
                            //outhtml << "  new google.maps.LatLng("<<point->lat<<", "<<point->lon<<"),\n";
                            QString lonlat = QString("%1,%2").arg(floorf(point->lat*100000)/100000).arg(floorf(point->lon*100000)/100000);
                            if (hash.contains(lonlat)) {
                                hash[lonlat] = hash[lonlat] + 1;
                            } else {
                                hash[lonlat] = 1;
                            }
                            maxCount = std::max(hash[lonlat], maxCount);

                            if (minLon > point->lon) minLon = point->lon;
                            if (minLat > point->lat) minLat = point->lat;
                            if (maxLon < point->lon) maxLon = point->lon;
                            if (maxLat < point->lat) maxLat = point->lat;
                        }
                    }
                }

                delete ride; // free memory!
            // open failed
            } else {
                current->setText(4, tr("Read error")); QApplication::processEvents();
            }
        }
    }

    QHashIterator<QString, int> i(hash);
    QString datapoints = "";
    while (i.hasNext()) {
         i.next();
         datapoints += QString("[%1,%2],")
                    .arg(i.key())
                    .arg(i.value());
    }
    QFile filehtml(dirName->text() + "/HeatMap.htm");
    filehtml.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream outhtml(&filehtml);
    if (mapProvider->currentIndex() == 0) { // OSM
        // Why openlayers instead of leaflet?
        // Throughout Golden Cheetah, leaflet is used for showing OSM-based maps and their overlays.
        // Leaflet provides a significant number of plugins, including some for creating heatmaps.
        // None of these heatmap plugins (https://leafletjs.com/plugins.html#heatmaps)
        // * are actively maintained as of this writing
        // * provide a heatmap comparable in power and flexibility to the one from openlayers
        //
        // openlayers is much more complex than leaflet, but comes with a layer for heatmaps out of
        // the box, providing better support in this special case.
        outhtml << "<!DOCTYPE html>\n";
        outhtml << "<html>\n";
        outhtml << " <head>\n";
        outhtml << "  <meta charset=\"UTF-8\">\n";
        outhtml << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        outhtml << "  <title>Golden Cheetah: Heatmap for filter</title>\n";
        outhtml << "  <script src=\"https://cdn.jsdelivr.net/npm/ol@v7.5.1/dist/ol.js\"></script>\n";
        outhtml << "  <link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/ol@v7.5.1/ol.css\">\n";
        outhtml << "  <style>\n";
        outhtml << "   body { margin: 0; padding: 0; }\n";
        outhtml << "   #map { position: absolute; top: 0; right: 0; bottom: 0; left: 0; }\n";
        outhtml << "  </style>\n";
        outhtml << " </head>\n";
        outhtml << " <body>\n";
        outhtml << "  <div id=\"map\"></div>\n";
        outhtml << "  <script>\n";
        outhtml << "   var rawData = [\n";
        outhtml << datapoints;
        outhtml << "   ];\n";
        outhtml << "   const vectorSource = new ol.source.Vector();\n";
        outhtml << "   for (let i = 0; i < rawData.length; ++i) {\n";
        outhtml << "    var coord = ol.proj.fromLonLat([rawData[i][1], rawData[i][0]]);\n";
        outhtml << "    var lonLat = new ol.geom.Point(coord);\n";
        outhtml << "    var pointFeature = new ol.Feature({\n";
        outhtml << "     geometry: lonLat,\n";
        outhtml << "     weight: 0.2 + 0.8 * rawData[i][2] / "<< maxCount << ".0\n";
        outhtml << "    });\n";
        outhtml << "    vectorSource.addFeature(pointFeature);\n";
        outhtml << "   }\n";
        outhtml << "   rawData.length = 0;\n";
        outhtml << "   const heatmap = new ol.layer.Heatmap({\n";
        outhtml << "    source: vectorSource,\n";
        outhtml << "    opacity: 1.5,\n";
        outhtml << "   });\n";
        outhtml << "   const attribution = new ol.control.Attribution({\n";
        outhtml << "    collapsible: false,\n";
        outhtml << "   });\n";
        outhtml << "   const map = new ol.Map({\n";
        outhtml << "    layers: [\n";
        outhtml << "     new ol.layer.Tile({\n";
        outhtml << "      source: new ol.source.OSM({\n";
        if (osmURLInput->text().trimmed().size() > 0) {
            outhtml << "       url: '" << osmURLInput->text().trimmed() << "/{z}/{x}/{y}.png',\n";
        }
        outhtml << "      }),\n";
        outhtml << "     }),\n";
        outhtml << "     heatmap,\n";
        outhtml << "    ],\n";
        outhtml << "    controls: ol.control.defaults.defaults({attribution: false}).extend([attribution]),\n";
        outhtml << "    target: 'map',\n";
        outhtml << "    view: new ol.View({\n";
        outhtml << "     constrainResolution: true,\n";
        outhtml << "    })\n";
        outhtml << "   });\n";
        outhtml << "   map.getView().on('change:resolution', (event) => {\n";
        outhtml << "    const radii = [1, 1.3, 2, 2.5, 4, 6.5, 11, 16, 25];\n";
        outhtml << "    let level = map.getView().getZoom();\n";
        outhtml << "    let index = Math.min(Math.max(0, Math.round(level) - 10), 8);\n";
        outhtml << "    heatmap.setRadius(radii[index]);\n";
        outhtml << "    heatmap.setBlur(radii[index]);\n";
        outhtml << "   });\n";
        outhtml << "   map.getView().fit(vectorSource.getExtent());\n";
        outhtml << "  </script>\n";
        outhtml << " </body>\n";
        outhtml << "</html>\n";
    } else { // Google Maps
        outhtml << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>Heatmaps</title>\n";
        outhtml << "<style>\n";
        outhtml << "html,body,#map-canvas {height: 100%;margin: 0px;padding: 0px;}</style>\n";
        if (googleAPIKeyInput->text().trimmed().size() > 0) {
            outhtml << "<script src=\"https://maps.googleapis.com/maps/api/js?v=3.exp&libraries=visualization&key=" << googleAPIKeyInput->text().trimmed() << "\"></script>\n";
        } else {
            outhtml << "<script src=\"https://maps.googleapis.com/maps/api/js?v=3.exp&libraries=visualization\"></script>\n";
        }
        outhtml << "<script>\n";
        outhtml << "var map,pointarray,heatmap;\n";
        outhtml << "var dataarray = [\n";
        outhtml << datapoints;
        outhtml << "];\n";
        outhtml << "var hmData = [];\n";
        outhtml << "function initialize() {\n";
        outhtml << "dataarray.forEach(function(entry) {hmData.push({location: new google.maps.LatLng(entry[0], entry[1]), weight: entry[2]});});\n";
        outhtml << "dataarray = [];\n";
        outhtml << "var mapOptions = { mapTypeId: google.maps.MapTypeId.SATELLITE};\n";
        outhtml << "map = new google.maps.Map(document.getElementById('map-canvas'),mapOptions);\n";
        outhtml << "var bounds = new google.maps.LatLngBounds();\n";
        outhtml << "bounds.extend(new google.maps.LatLng(" << minLat <<"," << minLon << "));\n";
        outhtml << "bounds.extend(new google.maps.LatLng(" << maxLat <<"," << maxLon << "));\n";
        outhtml << "map.fitBounds(bounds);\n";
        outhtml << "var pointArray = new google.maps.MVCArray(hmData);\n";
        outhtml << "heatmap = new google.maps.visualization.HeatmapLayer({data: pointArray, dissipating:true, maxIntensity:30, opacity:0.8});\n";
        outhtml << "google.maps.event.addListener(map,'zoom_changed',function() {\n";
        outhtml << "var zoomLevel = map.getZoom();\n";
        outhtml << "heatmap.set('radius',Math.ceil((Math.pow(zoomLevel,3))/200));\n";
        outhtml << "});\n";
        outhtml << "heatmap.setMap(map);\n";
        outhtml << "}\n";
        outhtml << "google.maps.event.addDomListener(window,'load',initialize);\n";
        outhtml << "</script></head><body><div id=\"map-canvas\"></div></body></html>\n";
    }
    filehtml.close();
}
