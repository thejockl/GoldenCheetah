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

#ifndef _GC_ScalingLabel_h
#define _GC_ScalingLabel_h 1

#include <QLabel>

class ScalingLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(int minFontPointSize READ getMinFontPointSize WRITE setMinFontPointSize USER false)
    Q_PROPERTY(int maxFontPointSize READ getMaxFontPointSize WRITE setMaxFontPointSize USER false)
    Q_PROPERTY(bool lazy READ isLazy WRITE setLazy USER false)

    public:
        ScalingLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
        ScalingLabel(int minFontPointSize, int maxFontPointSize, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~ScalingLabel();

        virtual void resizeEvent(QResizeEvent *evt);
        virtual void setFont(const QFont &font);

        int getMinFontPointSize() const;
        int getMaxFontPointSize() const;
        bool isLazy() const;

    public slots:
        void setText(const QString &text);
        void setMinFontPointSize(int size);
        void setMaxFontPointSize(int size);
        void setLazy(bool lazy);

    private:
        bool scaleFont(const QString &text, const QFont &font);
        bool scaleFont(const QString &text);
        bool scaleFontExact(const QString &text, const QFont &font);
        bool scaleFontLazy(const QString &text, const QFont &font);

        int minFontPointSize;
        int maxFontPointSize;
        bool lazy;
};

#endif