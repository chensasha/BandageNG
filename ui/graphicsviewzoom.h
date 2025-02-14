//Copyright 2017 Ryan Wick

//This file is part of Bandage

//Bandage is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//Bandage is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with Bandage.  If not, see <http://www.gnu.org/licenses/>.


#ifndef GRAPHICS_VIEW_ZOOM_H
#define GRAPHICS_VIEW_ZOOM_H

#include <QObject>
#include <QGraphicsView>

enum ZoomSource {MOUSE_WHEEL, SPIN_BOX, KEYBOARD, GESTURE};

// This class is a modified version of:
// http://stackoverflow.com/questions/19113532/qgraphicsview-zooming-in-and-out-under-mouse-position-using-mouse-wheel

/*!
 * This class adds ability to zoom QGraphicsView using mouse wheel. The point under cursor
 * remains motionless while it's possible.
 *
 * Note that it becomes not possible when the scene's
 * size is not large enough comparing to the viewport size. QGraphicsView centers the picture
 * when it's smaller than the view. And QGraphicsView's scrolls boundaries don't allow to
 * put any picture point at any viewport position.
 *
 * When the user starts scrolling, this class remembers original scene position and
 * keeps it until scrolling is completed. It's better than getting original scene position at
 * each scrolling step because that approach leads to position errors due to before-mentioned
 * positioning restrictions.
 *
 * When zoomed using scroll, this class emits zoomed() signal.
 *
 * Usage:
 *
 *   new GraphicsViewZoom(view);
 *
 * The object will be deleted automatically when the view is deleted.
 *
 * You can set keyboard modifiers used for zooming using set_modified(). Zooming will be
 * performed only on exact match of modifiers combination. The default modifier is Ctrl.
 *
 * You can change zoom velocity by calling setZoomFactorBase().
 * Zoom coefficient is calculated as zoom_factor_base^angle_delta
 * (see QWheelEvent::angleDelta).
 * The default zoom factor base is 1.0015.
 */

class MyGraphicsView;

class GraphicsViewZoom : public QObject
{
    Q_OBJECT
public:
    explicit GraphicsViewZoom(MyGraphicsView * view);
    void gentleZoom(double factor, ZoomSource zoomSource);
    void setModifiers(Qt::KeyboardModifiers modifiers);
    void setZoomFactorBase(double value);
    double m_zoomFactorBase;

private:
    MyGraphicsView * m_view;
    Qt::KeyboardModifiers m_modifiers;
    QPointF targetScenePos, targetViewportPos;
    bool eventFilter(QObject * object, QEvent * event) override;

signals:
    void zoomed();
};

#endif // GRAPHICS_VIEW_ZOOM_H
