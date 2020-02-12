
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

#include "meter.hpp"

// Qt include.
#include <QMultiMap>
#include <QPainter>
#include <QtMath>
#include <QVector>
#include <QRadialGradient>


//
// MeterPrivate
//

class MeterPrivate {
public:
	explicit MeterPrivate( Meter * parent )
		:  drawValue( true )
		,  drawGridValues( true )
		,  valuePrecision( 0 )
		,  scalePrecision( 0 )
		,  currentThreshold( 0 )
		,  radius( 100 )
		,  startScaleAngle( 30 )
		,  stopScaleAngle( 330 )
		,  minValue( 0.0 )
		,  maxValue( 100.0 )
		,  value( 0.0 )
		,  scaleStep( 1.0 )
		,  scaleGridStep( 10.0 )
		,  backgroundColor( Qt::black )
		,  needleColor( Qt::blue )
		,  textColor( Qt::white )
		,  gridColor( Qt::white )
		,  q( parent )
	{
	}

	struct RangeData {
		qreal start;
		qreal stop;
		QColor color;
	};

	struct DrawParams {
		QRectF rect;
		qreal scaleDegree;
		qreal startScaleAngle;
		qreal margin;
		qreal gridLabelSize;
		qreal scaleWidth;
		qreal fontPixelSize;
	};

	void drawBackground( QPainter & painter, DrawParams & params );
	void drawRanges( QPainter & painter, DrawParams & params );
	void drawScale( QPainter & painter, DrawParams & params );
	void drawLabels( QPainter & painter, DrawParams & params );
	void drawNeedle( QPainter & painter, DrawParams & params );

	bool thresholdFired();

	bool drawValue;
	bool drawGridValues;
	int valuePrecision;
	int scalePrecision;
	int currentThreshold;
	uint radius;
	uint startScaleAngle;
	uint stopScaleAngle;
	qreal minValue;
	qreal maxValue;
	qreal value;
	qreal scaleStep;
	qreal scaleGridStep;
	QColor backgroundColor;
	QColor needleColor;
	QColor textColor;
	QColor gridColor;
	QString label;
	QString unitsLabel;
	QMultiMap< int, RangeData > ranges;
	Meter * q;
}; // class MeterPrivate

void
MeterPrivate::drawBackground( QPainter & painter, DrawParams & params )
{
	painter.save();
	painter.setPen( backgroundColor );
	painter.setBrush( backgroundColor );
	params.rect = QRectF( 0.0, 0.0, radius * 2, radius * 2 );
	painter.drawEllipse( params.rect );
	painter.restore();
}

void
MeterPrivate::drawRanges( QPainter & painter, DrawParams & params )
{
	params.scaleDegree = stopScaleAngle - startScaleAngle;
	params.startScaleAngle = startScaleAngle;
	params.margin = radius / 20.0;

	const qreal gridLabelSizeFactor = 10.0;

	params.scaleWidth = radius / ( gridLabelSizeFactor + 20.0 );
	params.gridLabelSize = radius / gridLabelSizeFactor;
	params.fontPixelSize = params.gridLabelSize * 0.75;

	const qreal m = params.margin + params.scaleWidth / 2.0;

	const QRectF r = params.rect - QMarginsF( m, m, m,m );

	painter.save();

	for( auto it = ranges.cbegin(), last = ranges.cend(); it != last; ++it )
	{
		painter.setPen( QPen( it.value().color, params.scaleWidth ) );
		const qreal angle = params.startScaleAngle +
			params.scaleDegree * ( it.value().start / ( maxValue - minValue ) );
		const qreal span = params.startScaleAngle +
			params.scaleDegree * ( it.value().stop / ( maxValue - minValue ) ) - angle;
		painter.drawArc( r, ( -90.0 - angle ) * 16, -span * 16 );
	}

	painter.restore();
}

void
MeterPrivate::drawScale( QPainter & painter, DrawParams & params )
{
	painter.save();
	painter.setPen( textColor );
	painter.drawArc( params.rect -
	QMarginsF( params.margin, params.margin, params.margin, params.margin ),
		( -90.0 - params.startScaleAngle ) * 16, -params.scaleDegree * 16 );
	painter.translate( radius, radius );
	painter.rotate( params.startScaleAngle );

	QVector< qreal > alreadyDrawn;
	alreadyDrawn.append( params.startScaleAngle );

	if( scaleGridStep > 0.0 )
	{
		const int stepsCount = ( ( maxValue - minValue ) / scaleGridStep );
		const qreal scaleStepInDegree = params.scaleDegree / stepsCount;

		for( int i = 0; i <= stepsCount; ++i )
		{
			painter.drawLine( 0, radius - params.margin, 0,
			radius - params.gridLabelSize - params.margin );
			painter.rotate( scaleStepInDegree );
			alreadyDrawn.append( params.startScaleAngle + i * scaleStepInDegree );
		}
	}
	else
	{
		painter.drawLine( 0, radius - params.margin, 0,
		radius - params.gridLabelSize - params.margin );

		painter.rotate( params.scaleDegree );

		alreadyDrawn.append( params.scaleDegree );

		painter.drawLine( 0, radius - params.margin, 0,
		radius - params.gridLabelSize - params.margin );
	}

	painter.restore();

	if( scaleStep > 0.0 )
	{
		painter.save();
		painter.setPen( textColor );
		painter.translate( radius, radius );
		painter.rotate( params.startScaleAngle );

		int stepsCount = ( ( maxValue - minValue ) / scaleStep );
		const qreal scaleStepInDegree = params.scaleDegree / stepsCount;

		stepsCount -= 2;

		painter.rotate( scaleStepInDegree );

		for( int i = 0; i <= stepsCount; ++i )
		{
			if( !alreadyDrawn.contains( ( params.startScaleAngle + scaleStepInDegree )
				+ i * scaleStepInDegree ) )
					painter.drawLine( 0, radius - params.margin, 0,
						radius - params.scaleWidth - params.margin );

			painter.rotate( scaleStepInDegree );
		}

		painter.restore();
	}
}

void
MeterPrivate::drawLabels( QPainter & painter, DrawParams & params )
{
	if( scaleGridStep > 0.0 && drawGridValues )
	{
		painter.save();
		painter.translate( radius, radius );
		painter.setPen( textColor );

		qreal startRad = - qDegreesToRadians( (qreal) startScaleAngle  );
		int stepsCount = ( ( maxValue - minValue ) / scaleGridStep );
		qreal deltaRad = - qDegreesToRadians( params.scaleDegree / stepsCount );
		qreal sina, cosa;
		qreal val = minValue;

		QFont f = painter.font();
		f.setPixelSize( params.fontPixelSize );
		painter.setFont( f );
		QFontMetricsF fm( f );

		for ( int i = 0; i <= stepsCount; ++i )
		{
			sina = qSin( startRad + i * deltaRad );
			cosa = qCos( startRad + i * deltaRad );

			const QString str = QString::number( val, 'f', scalePrecision );

			const QSizeF s = fm.size( Qt::TextSingleLine, str );

			const qreal offset = ( radius - params.gridLabelSize - params.margin * 3 );

			const int x = ( offset * sina ) - ( s.width() / 2 );
			const int y = ( offset * cosa ) + ( s.height() / 4 );

			painter.drawText( x, y, str );

			val += scaleGridStep;
		}

		painter.restore();
	}

	if( !unitsLabel.isEmpty() )
	{
		painter.save();
		QFont f = painter.font();
		f.setPixelSize( params.fontPixelSize );
		painter.setFont( f );
		painter.setPen( textColor );
		painter.drawText( QRectF( 0, params.margin * 3 + params.gridLabelSize * 3,
			radius * 2, radius ), unitsLabel, QTextOption( Qt::AlignHCenter ) );
		painter.restore();
	}

	if( !label.isEmpty() )
	{
		painter.save();
		QFont f = painter.font();
		f.setPixelSize( params.fontPixelSize );
		painter.setFont( f );
		painter.setPen( textColor );
		painter.drawText( QRectF( 0, radius * 2 - params.margin * 3 - params.gridLabelSize * 3,
			radius * 2, radius ), label, QTextOption( Qt::AlignHCenter ) );
		painter.restore();
	}

	if( drawValue )
	{
		painter.save();
		QFont f = painter.font();
		f.setPixelSize( params.fontPixelSize * 2 );
		f.setBold( true );
		painter.setFont( f );
		painter.setPen( textColor );
		painter.drawText( QRectF( 0, radius * 2 - params.margin - params.gridLabelSize * 2,
			radius * 2, radius ), QString::number( value, 'f', valuePrecision ),
			QTextOption( Qt::AlignHCenter ) );
		painter.restore();
	}
}

void
MeterPrivate::drawNeedle( QPainter & painter, DrawParams & params )
{
	const qreal r = radius / 10.0;

	painter.save();
	painter.translate( radius, radius );
	painter.rotate( params.startScaleAngle + params.scaleDegree * value / ( maxValue - minValue ) );
	painter.setPen( QPen( needleColor, radius / 75.0 ) );
	painter.drawLine( 0, radius - params.margin, 0, - ( r * 2.0 ) );
	painter.setBrush( backgroundColor );
	painter.setPen( Qt::NoPen );
	painter.drawEllipse( -r, -r, r * 2.0, r * 2.0 );
	painter.restore();

	painter.save();
	const auto c = backgroundColor.redF() + backgroundColor.greenF() + backgroundColor.blueF();
	painter.translate( radius, radius );
	QRadialGradient gradient( 0.0, 0.0, r, r, r );

	if( c < 1.0 )
	{
		gradient.setColorAt( 0.0, textColor );
		gradient.setColorAt( 1.0, Qt::transparent );
	}
	else
	{
		gradient.setColorAt( 1.0, textColor );
		gradient.setColorAt( 0.0, Qt::transparent );
	}

	painter.setBrush( gradient );
	painter.setPen( Qt::NoPen );
	painter.drawEllipse( -r, -r, r * 2.0, r * 2.0 );
	painter.restore();
}

bool
MeterPrivate::thresholdFired()
{
	bool res = false;

	for( auto it = ranges.cbegin(), last = ranges.cend(); it != last; ++it )
	{
		if( value >= it.value().start && value < it.value().stop )
		{
			if( currentThreshold != it.key() )
				res = true;

			currentThreshold = it.key();

			break;
		}
	}

	return res;
}


//
// Meter
//

Meter::Meter( QWidget * parent )
	:  QWidget( parent )
	,  d( new MeterPrivate( this ) )
{
	setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}

Meter::~Meter()
{
}

qreal
Meter::minValue() const
{
	return d->minValue;
}

void
Meter::setMinValue( qreal v )
{
	d->minValue = v;

	if( d->minValue > d->maxValue )
		d->maxValue = d->minValue;

	update();
}

qreal
Meter::maxValue() const
{
	return d->maxValue;
}

void
Meter::setMaxValue( qreal v )
{
	d->maxValue = v;

	if( d->minValue > d->maxValue )
		d->minValue = d->maxValue;

	update();
}

qreal
Meter::value() const
{
	return d->value;
}

void
Meter::setValue( qreal v )
{
	if( v >= d->minValue && v <= d->maxValue )
	{
		d->value = v;

		update();

		emit valueChanged( v );

		if( d->thresholdFired() )
			emit thresholdFired( d->currentThreshold );
	}
}

const QColor &
Meter::backgroundColor() const
{
	return d->backgroundColor;
}

void
Meter::setBackgroundColor( const QColor & c )
{
	d->backgroundColor = c;

	update();
}

const QColor &
Meter::needleColor() const
{
	return d->needleColor;
}

void
Meter::setNeedleColor( const QColor & c )
{
	d->needleColor = c;

	update();
}

const QColor &
Meter::textColor() const
{
	return d->textColor;
}

void
Meter::setTextColor( const QColor & c )
{
	d->textColor = c;

	update();
}

const QColor &
Meter::gridColor() const
{
	return d->gridColor;
}

void
Meter::setGridColor( const QColor & c )
{
	d->gridColor = c;

	update();
}

const QString &
Meter::label() const
{
	return d->label;
}

void
Meter::setLabel( const QString & l )
{
	d->label = l;

	update();
}

const QString
Meter::unitsLabel() const
{
	return d->unitsLabel;
}

void
Meter::setUnitsLabel( const QString & l )
{
	d->unitsLabel = l;

	update();
}

uint
Meter::radius() const
{
	return d->radius;
}

void
Meter::setRadius( uint r )
{
	if( r < 45 )
		r = 45;

	d->radius = r;

	resize( sizeHint() );

	update();
}

uint
Meter::startScaleAngle()
{
	return d->startScaleAngle;
}

void
Meter::setStartScaleAngle( uint a )
{
	d->startScaleAngle = a;

	update();
}

uint
Meter::stopScaleAngle() const
{
	return d->stopScaleAngle;
}

void
Meter::setStopScaleAngle( uint a )
{
	d->stopScaleAngle = a;

	update();
}

qreal
Meter::scaleStep() const
{
	return d->scaleStep;
}

void
Meter::setScaleStep( qreal s )
{
	if( s >= 0.0 )
	{
		d->scaleStep = s;

		update();
	}
}

qreal
Meter::scaleGridStep() const
{
	return d->scaleGridStep;
}

void
Meter::setScaleGridStep( qreal s )
{
	if( s >= 0.0 )
	{
		d->scaleGridStep = s;

		update();
	}
}

bool
Meter::drawValue() const
{
	return d->drawValue;
}

void
Meter::setDrawValue( bool on )
{
	d->drawValue = on;

	update();
}

int
Meter::drawValuePrecision() const
{
	return d->valuePrecision;
}

void
Meter::setDrawValuePrecision( int p )
{
	if( p >= 0 )
	{
		d->valuePrecision = p;

		update();
	}
}

int
Meter::scaleLabelPrecision() const
{
	return d->scalePrecision;
}

void
Meter::setScaleLabelPrecision( int p )
{
	if( p >= 0 )
	{
		d->scalePrecision = p;

		update();
	}
}

bool
Meter::drawGridValues() const
{
	return d->drawGridValues;
}

void
Meter::setDrawGridValues( bool on )
{
	d->drawGridValues = on;

	update();
}

void
Meter::setThresholdRange( qreal start, qreal stop, int thresholdIndex,
	const QColor & color )
{
	d->ranges.insert( thresholdIndex, { start, stop, color } );

	if( d->thresholdFired() )
		emit thresholdFired( d->currentThreshold );

	update();
}

QSize
Meter::minimumSizeHint() const
{
	return QSize( d->radius * 2 + 2, d->radius * 2 + 2 );
}

QSize
Meter::sizeHint() const
{
	return minimumSizeHint();
}

void
Meter::paintEvent( QPaintEvent * )
{
	QPainter p( this );
	p.setRenderHint( QPainter::Antialiasing );
	p.translate( 1.0, 1.0 );

	MeterPrivate::DrawParams params;

	d->drawBackground( p, params );
	d->drawRanges( p, params );
	d->drawScale( p, params );
	d->drawLabels( p, params );
	d->drawNeedle( p, params );
}
