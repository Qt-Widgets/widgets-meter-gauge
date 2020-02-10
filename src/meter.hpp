
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2020 Igor Mironchik

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef METER_HPP_INCLUDED
#define METER_HPP_INCLUDED

// Qt include.
#include <QWidget>
#include <QScopedPointer>


//
// Meter
//

class MeterPrivate;

//! Meter widget.
class Meter Q_DECL_FINAL
:  public QWidget
{
	Q_OBJECT

	Q_PROPERTY( qreal minValue READ minValue WRITE setMinValue )
	Q_PROPERTY( qreal maxValue READ maxValue WRITE setMaxValue )
	Q_PROPERTY( qreal value READ value WRITE setValue )
	Q_PROPERTY( QColor backgroundColor READ backgroundColor WRITE setBackgroundColor )
	Q_PROPERTY( QColor needleColor READ needleColor WRITE setNeedleColor )
	Q_PROPERTY( QColor textColor READ textColor WRITE setTextColor )
	Q_PROPERTY( QColor gridColor READ gridColor WRITE setGridColor )
	Q_PROPERTY( QString label READ label WRITE setLabel )
	Q_PROPERTY( QString unitsLabel READ unitsLabel WRITE setUnitsLabel )
	Q_PROPERTY( uint radius READ radius WRITE setRadius )
	Q_PROPERTY( uint startScaleAngle READ startScaleAngle WRITE setStartScaleAngle )
	Q_PROPERTY( uint stopScaleAngle READ stopScaleAngle WRITE setStopScaleAngle )
	Q_PROPERTY( qreal scaleStep READ scaleStep WRITE setScaleStep )
	Q_PROPERTY( qreal scaleGridStep READ scaleGridStep WRITE setScaleGridStep )
	Q_PROPERTY( bool drawValue READ drawValue WRITE setDrawValue )
	Q_PROPERTY( bool drawGridValues READ drawGridValues WRITE setDrawGridValues )
	Q_PROPERTY( int drawValuePrecision READ drawValuePrecision WRITE setDrawValuePrecision )
	Q_PROPERTY( int scaleLabelPrecision READ scaleLabelPrecision WRITE setScaleLabelPrecision )

signals:
	//! Value changed.
	void valueChanged( qreal currentValue );
	//! Threshold.
	void thresholdFired( int thresholdIndex );

public:
	Meter( QWidget * parent = Q_NULLPTR );
	virtual ~Meter();

	qreal minValue() const;
	void setMinValue( qreal v );

	qreal maxValue() const;
	void setMaxValue( qreal v );

	qreal value() const;

	const QColor & backgroundColor() const;
	void setBackgroundColor( const QColor & c );

	const QColor & needleColor() const;
	void setNeedleColor( const QColor & c );

	const QColor & textColor() const;
	void setTextColor( const QColor & c );

	const QColor & gridColor() const;
	void setGridColor( const QColor & c );

	const QString & label() const;
	void setLabel( const QString & l );

	const QString unitsLabel() const;
	void setUnitsLabel( const QString & l );

	uint radius() const;
	void setRadius( uint r );

	uint startScaleAngle();
	void setStartScaleAngle( uint a );

	uint stopScaleAngle() const;
	void setStopScaleAngle( uint a );

	qreal scaleStep() const;
	void setScaleStep( qreal s );

	qreal scaleGridStep() const;
	void setScaleGridStep( qreal s );

	bool drawValue() const;
	void setDrawValue( bool on = true );

	int drawValuePrecision() const;
	void setDrawValuePrecision( int p );

	int scaleLabelPrecision() const;
	void setScaleLabelPrecision( int p );

	bool drawGridValues() const;
	void setDrawGridValues( bool on = true );

	/*!
		\brief Set threshold range [start, stop).

		\param start Start value of range.
		\param stop Stop value of range
		\param thresholdIndex Index of the threshold.
		\param color Color used to draw range on the scale.
	*/
	void setThresholdRange( qreal start, qreal stop, int thresholdIndex,
		const QColor & color = Qt::transparent );

	QSize minimumSizeHint() const Q_DECL_OVERRIDE;
	QSize sizeHint() const Q_DECL_OVERRIDE;

	public slots:
	void setValue( qreal v );

	protected:
	void paintEvent( QPaintEvent * ) Q_DECL_OVERRIDE;

private:
	Q_DISABLE_COPY( Meter )

	QScopedPointer< MeterPrivate > d;
}; // class Meter

#endif // METER_HPP_INCLUDED
