/*
 * Copyright (c) 2022 Joachim Kohlhammer (joachim.kohlhammer@gmx.de)
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

#ifndef _GC_TrainHrController_h
#define _GC_TrainHrController_h 1

#include <QList>

class TrainHrController
{
    public:
        TrainHrController(int bufSize = 20, int powerStep = 1, int waitingPeriod = 3000);

        void reset();
        void setCp(int cp);
        void setTargetHr(int pTargetHr);
        void incTargetHr(int by);
        void decTargetHr(int by);
        int getTargetHr() const;

        int getLoad(int hr, int lastLoad, long nowMsecs);

    protected:
        /**
         * Evaluate whether and how much hr is above or below targetHr.
         * @param hr Heartrate, usually an average over the past n heartrates
         * @return <0 if hr is below targetHr, >0 if hr is above targetHr, 0 if hr equals targetHr.
         *         The value is farther off 0 the farther hr is off targetHr.
         */
        int categorizeHr(int hr) const;
        double getAvg(const QList<int> &data) const;
        double getWeightedAvg(const QList<int> &data) const;

    private:
        int targetHr;
        int cp;
        long lastChangeAt;
        int lastTargetLoad;
        QList<int> hrHistory;
        QList<int> loadHistory;

        const int bufSize;
        const int powerStep;
        const int waitingPeriod;
};

#endif // _GC_TrainHrController_h
