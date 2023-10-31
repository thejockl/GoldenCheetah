#include "TrainHrController.h"

#include <QtGlobal>
#include <QDebug>
#include <cmath>


TrainHrController::TrainHrController
(int bufSize, int powerStep, int waitingPeriod)
: targetHr(0), cp(0), lastChangeAt(0), lastTargetLoad(0), hrHistory(), loadHistory(), bufSize(bufSize), powerStep(powerStep), waitingPeriod(waitingPeriod)
{
}


void
TrainHrController::reset
()
{
    hrHistory.clear();
    loadHistory.clear();
    lastChangeAt = 0;
    targetHr = 0;
    cp = 0;
    lastTargetLoad = 0;
}


void
TrainHrController::setCp
(int cp)
{
    this->cp = cp;
}


void
TrainHrController::setTargetHr
(int pTargetHr)
{
    targetHr = qMin(pTargetHr, 180);
    targetHr = qMax(targetHr, 100);
}


void
TrainHrController::incTargetHr
(int by)
{
    setTargetHr(targetHr + by);
}


void
TrainHrController::decTargetHr
(int by)
{
    setTargetHr(targetHr - by);
}


int
TrainHrController::getTargetHr
() const
{
    return targetHr;
}


int
TrainHrController::getLoad
(int hr, int lastLoad, long msecs)
{
    if (hr > 0) {
        hrHistory.append(hr);
        if (hrHistory.size() > bufSize) {
            hrHistory.pop_front();
        }
    }
    int newLoad = lastLoad;
    if (lastTargetLoad > 0) {
        if (   msecs - lastChangeAt > waitingPeriod
            && hrHistory.size() == bufSize) {
            int cat = categorizeHr(getWeightedAvg(hrHistory));
            if (   (hr >= targetHr && cat < 0)
                || (hr <= targetHr && cat > 0)) {
                cat = 0;
            }
            qDebug() << "hr =" << hr << "/ targetHr =" << targetHr << "/ weightedAvg =" << getWeightedAvg(hrHistory) << "/ cat =" << cat << "/ avg =" << getAvg(hrHistory);
            newLoad = lastLoad - (cat * powerStep);
        }
    } else {
        newLoad = cp / 2;
    }

    newLoad = qMax(50, newLoad);
    newLoad = qMin(1500, newLoad);
    lastTargetLoad = newLoad;
    if (newLoad != lastLoad) {
        lastChangeAt = msecs;
        qDebug() << "Set new load to " << newLoad;
    }
    loadHistory.append(newLoad);

    return newLoad;
}


int
TrainHrController::categorizeHr
(int hr) const
{
    float deviation = fabs((targetHr - hr) / float(targetHr));
    int signedFactor = hr > targetHr ? 2 : -1;
    if (deviation > 0.1) {
        return 3 * signedFactor;
    } else if (deviation > 0.05) {
        return 2 * signedFactor;
    } else if (deviation > 0.02) {
        return 1 * signedFactor;
    }
    return 0;
}


double
TrainHrController::getAvg
(const QList<int> &data) const
{
    long sum = 0;
    for (int v: data) {
        sum += v;
    }
    return sum / double(data.size());
}


double
TrainHrController::getWeightedAvg
(const QList<int> &data) const
{
    long sum = 0;
    long count = 0;
    for (int i = 0; i < data.size(); ++i) {
        int w = (i + 1) * (i + 1);
        sum += w * data[i];
        count += w;
    }
    return sum / double(count);
}
