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

#include "ScalingLabel.h"

#include <QDebug>


ScalingLabel::ScalingLabel
(QWidget *parent, Qt::WindowFlags f)
:ScalingLabel(4, 64, parent, f)
{
}



ScalingLabel::ScalingLabel
(int minFontPointSize, int maxFontPointSize, QWidget *parent, Qt::WindowFlags f)
: QLabel(parent, f), minFontPointSize(minFontPointSize), maxFontPointSize(maxFontPointSize), lazy(true)
{
    QFont fnt = font();
    fnt.setPointSize(minFontPointSize);
    QLabel::setFont(fnt);
}


ScalingLabel::~ScalingLabel
()
{
}


void
ScalingLabel::resizeEvent
(QResizeEvent *)
{
    scaleFont(text());
}


void
ScalingLabel::setText
(const QString &text)
{
    if (text.length() > QLabel::text().length()) {
        scaleFont(text);
    }
    QLabel::setText(text);
}


void
ScalingLabel::setFont
(const QFont &font)
{
    if (! scaleFont(text(), font)) {
        QLabel::setFont(font);
    }
}


int
ScalingLabel::getMinFontPointSize
() const
{
    return minFontPointSize;
}


void
ScalingLabel::setMinFontPointSize
(int size)
{
    minFontPointSize = size;
}


int
ScalingLabel::getMaxFontPointSize
() const
{
    return maxFontPointSize;
}


void
ScalingLabel::setMaxFontPointSize
(int size)
{
    maxFontPointSize = size;
}


bool
ScalingLabel::isLazy
() const
{
    return lazy;
}


void
ScalingLabel::setLazy
(bool lazy)
{
    this->lazy = lazy;
}


bool
ScalingLabel::scaleFont
(const QString &text)
{
    return scaleFont(text, font());
}


bool
ScalingLabel::scaleFont
(const QString &text, const QFont &font)
{
    if (lazy) {
        return scaleFontLazy(text, font);
    } else {
        return scaleFontExact(text, font);
    }
}


bool
ScalingLabel::scaleFontExact
(const QString &text, const QFont &font)
{
    int size = maxFontPointSize;
    QFont f(font);
    do {
        f.setPointSize(size);
        QFontMetrics fm = QFontMetrics(f, this);
        QRect br = fm.boundingRect(text);
        if (br.width() <= width() && br.height() <= height()) {
            break;
        }
        --size;
    } while (size >= minFontPointSize);
    QLabel::setFont(f);
    return true;
}


bool
ScalingLabel::scaleFontLazy
(const QString &text, const QFont &font)
{
    if (text.length() == 0 || width() <= 0 || height() <= 0) {
        return false;
    }
    QFont f(font);
    f.setPointSize(minFontPointSize);
    QFontMetrics fmS = QFontMetrics(f, this);
    f.setPointSize(maxFontPointSize);
    QFontMetrics fmL = QFontMetrics(f, this);
    QRect brS = fmS.boundingRect(text);
    QRect brL = fmL.boundingRect(text);
    int sizeWidth = (maxFontPointSize - minFontPointSize) / float(brL.width() - brS.width()) * width();
    int sizeHeight = (maxFontPointSize - minFontPointSize) / float(brL.height() - brS.height()) * height();
    f.setPointSize(qMax(qMin(qMin(sizeWidth, sizeHeight), maxFontPointSize), minFontPointSize));
    QLabel::setFont(f);
    return true;
}
