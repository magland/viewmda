#include "viewmdawidget2d.h"
#include <QImage>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <math.h>

ViewmdaWidget2D::ViewmdaWidget2D(QWidget *parent) : QWidget(parent)
{
	m_need_to_update_the_image=true;
	m_model=0;
	setFocusPolicy(Qt::StrongFocus);
}

ViewmdaWidget2D::~ViewmdaWidget2D()
{
}

void ViewmdaWidget2D::setModel(ViewmdaModel *M) {
	m_model=M;
	
	connect(m_model,SIGNAL(arrayChanged()),this,SLOT(slot_array_changed()));
	connect(m_model,SIGNAL(dimensionsChanged()),this,SLOT(slot_dimensions_changed()));
	connect(m_model,SIGNAL(currentIndexChanged()),this,SLOT(slot_current_index_changed()));
	connect(m_model,SIGNAL(currentSliceChanged()),this,SLOT(slot_current_slice_changed()));
	connect(m_model,SIGNAL(selectedPointsChanged()),this,SLOT(slot_selected_points_changed()));
	connect(m_model,SIGNAL(selectedRectChanged()),this,SLOT(slot_selected_rect_changed()));
	connect(m_model,SIGNAL(zoomRectChanged()),this,SLOT(slot_zoom_rect_changed()));
	connect(m_model,SIGNAL(windowsChanged()),this,SLOT(slot_windows_changed()));
}

void ViewmdaWidget2D::paintEvent ( QPaintEvent * event ) {
	QPainter painter(this);
	if (!m_model) return;
	if (m_need_to_update_the_image) {
		m_need_to_update_the_image=false;
		m_the_image=QImage(1,1,QImage::Format_RGB32);
		if ((m_model->N1()>0)&&(m_model->N2()>0)) {
			m_the_image=QImage(m_model->N1(),m_model->N2(),QImage::Format_RGB32);
			for (qint32 x=0; x<m_model->N1(); x++)
				for (qint32 y=0; y<m_model->N2(); y++) {
					QColor col=get_color(m_model->get(x,y),m_model->getMask(x,y),m_model->hue(x,y),m_model->saturation(x,y));
					m_the_image.setPixel(x,y,qRgb(col.red(),col.green(),col.blue()));
				}
		}
	}	
	int iwidth=m_the_image.width();
	int iheight=m_the_image.height();
	if (width()*iheight<iwidth*height()) { //width is the limiting direction
		int target_height=(int)(width()*iheight/(double)iwidth);
		m_target_rect=QRect(0,(height()-target_height)/2,width(),target_height);
	}
	else { //height is the limiting direction
		int target_width=(int)(height()*iwidth/(double)iheight);
		m_target_rect=QRect((width()-target_width)/2,0,target_width,height());
	}
	painter.drawImage(m_target_rect,m_the_image,QRect(0,0,m_the_image.width(),m_the_image.height()));
	draw_current_position(painter);
}

QColor ViewmdaWidget2D::get_color(Complex X,unsigned char M,int hue,float saturation) {
	if (!m_model) return qRgb(0,0,0);
	float wmin=m_model->windowMin();
	float wmax=m_model->windowMax();
	real val=abs(X);
	int hold=0;
	if (wmin==wmax) {
		if (val<wmin) hold=0;
		else if (val>wmax) hold=255;
		else hold=127;
	}
	else if (wmax>wmin) {
		if (val<wmin) val=wmin;
		if (val>wmax) val=wmax;
		hold=(int)((val-wmin)/(wmax-wmin)*255);
	}
	if (M==0) {
		float swmin=m_model->saturationWindowMin();
		float swmax=m_model->saturationWindowMax();
		int sat=0;
		int hue=0;
		if (saturation<0) {
			saturation=-saturation;
			hue=(hue+180)%360;
		}
		if (swmax>swmin) sat=(int)((saturation-swmin)/(swmax-swmin)*255);
		//return QColor::fromHsv(hue,sat,hold);
		return qRgb(hold,hold,hold);
	}
	else {
		if (M==1) return qRgb(hold,hold,255);
		else if (M==2) return qRgb(hold,255,hold);
		else if (M==3) return qRgb(255,hold,hold);
		else return qRgb(255,255,hold);
	}
	
}


void ViewmdaWidget2D::slot_array_changed() {
	m_need_to_update_the_image=true;
	repaint();
}
void ViewmdaWidget2D::slot_dimensions_changed() {
	m_need_to_update_the_image=true;
	repaint();
}
void ViewmdaWidget2D::slot_current_index_changed() {
	repaint();
}
void ViewmdaWidget2D::slot_current_slice_changed() {
	m_need_to_update_the_image=true;
	repaint();
}
void ViewmdaWidget2D::slot_selected_points_changed() {
	m_need_to_update_the_image=true;
	repaint();
}
void ViewmdaWidget2D::slot_selected_rect_changed() {
	repaint();
}
void ViewmdaWidget2D::slot_zoom_rect_changed() {
	m_need_to_update_the_image=true;
	repaint();
}

void ViewmdaWidget2D::draw_current_position(QPainter &painter) {
	if (!m_model) return;
	painter.setPen(QPen(QColor(200,0,200),3));
	
	if (m_model->polygonMode()) {
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Draw polygon
		m_polygon_point_rects.clear();
		m_polygon_point_lines.clear();
		PolygonSlices *PS=&m_model->polygonData().m_polygon_slices;
		if (PS->pointCount()>0) {
			int slice_number=m_model->currentIndex(m_model->D3());
			QList<QPoint> pts;
			for (int j=0; j<PS->pointCount(); j++) {
				pts << PS->point(slice_number,j);	
			}
			for (int j=0; j<pts.count(); j++) {
				int px1,py1,px2,py2;
				QPoint hold;
				if (j==0) hold=indexToPoint(pts[pts.count()-1]); 
				else hold=indexToPoint(pts[j-1]);
				px1=hold.x(); py1=hold.y();
				hold=indexToPoint(pts[j]);
				px2=hold.x(); py2=hold.y();
				painter.setPen(QPen(QColor(155,0,255),2));
				QLine line=QLine(px1,py1,px2,py2);
				painter.drawLine(line);
				m_polygon_point_lines << line;
				if (m_model->polygonData().m_polygon_fixed) painter.setPen(QPen(QColor(255,0,255),2));
				if (m_model->polygonData().m_selected_polygon_point==j) painter.setPen(QPen(QColor(0,255,255),2));
				QRect rect=QRect(px2-4,py2-4,9,9);
				painter.drawEllipse(rect);
				m_polygon_point_rects << rect;
			}
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	else {
		if (m_model->selectedRect().x()<0) {
			QPoint pt=indexToPoint(QPoint(m_model->C1(),m_model->C2()));
			painter.drawLine(pt.x()-5,pt.y(),pt.x()+5,pt.y());
			painter.drawLine(pt.x(),pt.y()-5,pt.x(),pt.y()+5);
		}
		else {
			QRect SR=m_model->selectedRect();
			QPoint pt1=indexToPoint(QPoint(SR.x(),SR.y()));
			QPoint pt2=indexToPoint(QPoint(SR.x()+SR.width(),SR.y()+SR.height()));
			if (m_model->ellipticalSelection())
				painter.drawEllipse(QRect(pt1.x(),pt1.y(),pt2.x()-pt1.x(),pt2.y()-pt1.y()));
			else
				painter.drawRect(QRect(pt1.x(),pt1.y(),pt2.x()-pt1.x(),pt2.y()-pt1.y()));
			
		}
	}
}

QPoint ViewmdaWidget2D::indexToPoint(QPoint index) {
	if (!m_model) return QPoint(0,0);
	if (m_model->N1()==0) return QPoint(0,0);
	if (m_model->N2()==0) return QPoint(0,0);
	//index.setY(m_model->N2()-1-index.y());
	QPoint pt=QPoint(m_target_rect.x()+(index.x()+0.5)/m_model->N1()*m_target_rect.width(),m_target_rect.y()+(index.y()+0.5)/m_model->N2()*m_target_rect.height());
	return pt;
}

QPoint ViewmdaWidget2D::pointToIndex(QPoint pt) {
	if (!m_model) return QPoint(0,0);
	if (m_target_rect.width()==0) return QPoint(0,0);
	if (m_target_rect.height()==0) return QPoint(0,0);
	float pctx=((float)pt.x()-m_target_rect.x())/(m_target_rect.width());
	float pcty=((float)pt.y()-m_target_rect.y())/(m_target_rect.height());
	int retx=(int)(pctx*m_model->N1());
	int rety=(int)(pcty*m_model->N2());
	if (retx<0) retx=0;
	if (retx>=m_model->N1())
		retx=m_model->N1()-1;
	if (rety<0) rety=0;
	if (rety>=m_model->N2())
		rety=m_model->N2()-1;
	return QPoint(retx,/*m_model->N2()-1-*/rety);
}

void ViewmdaWidget2D::mousePressEvent ( QMouseEvent * event ) {
	if (!m_model) return;
	if ((event->button()&Qt::LeftButton)||(event->button()&Qt::RightButton)) {
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Polygon mode
		if (m_model->polygonMode()) {
			if (event->button()&Qt::LeftButton) {
				PolygonSlices *PS=&m_model->polygonData().m_polygon_slices;
				if (!m_model->polygonData().m_polygon_fixed) {
					long i1,i2;
					QPoint hold=pointToIndex(event->pos());
					i1=hold.x(); i2=hold.y();
					int current_slice=m_model->currentIndex(m_model->D3());
					PS->insertPoint(current_slice,PS->pointCount(),QPoint(i1,i2));
					repaint();
				}
				else  { //polygon fixed mode
					int selected_point=-1;
					bool found=false;
					for (int j=0; j<m_polygon_point_rects.count(); j++)
						if (m_polygon_point_rects[j].contains(event->pos())) {
							selected_point=j;
							found=true;
						}
					m_model->polygonData().m_selected_polygon_point=selected_point;
					if (!found) {
						int selected_line_index=-1;
						for (int j=0; (j<m_polygon_point_lines.count())&&(!found); j++) {
							QLine line=m_polygon_point_lines[j];
							QPoint pt=event->pos();
							int dx=(line.x2()-line.x1());
							int dy=(line.y2()-line.y1());
							int dxb=pt.x()-line.x1();
							int dyb=pt.y()-line.y1();
							if ((dx!=0)||(dy!=0)) {
								double t=(dx*dxb+dy*dyb)/((double)dx*dx+dy*dy);
								if ((0.33<=t)&&(t<=0.66)) {
									double pt2x=line.x1()+t*dx;
									double pt2y=line.y1()+t*dy;
									double dist=sqrt((pt2x-pt.x())*(pt2x-pt.x())+(pt2y-pt.y())*(pt2y-pt.y()));
									if (dist<=3) {
										selected_line_index=j;
										found=true;
									}
								}
							}
						}
						if (selected_line_index>=0) {
							PS->insertPoint(selected_line_index);
							m_model->polygonData().m_selected_polygon_point=selected_line_index;
						}
					}
					repaint();
				}
			}
			else if (event->button()&Qt::RightButton) {
				m_model->polygonData().m_polygon_fixed=true;
				repaint();
			}
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		else {
			m_model->setSelectedRect(QRect(-1,-1,1,1),false);
			QPoint index=pointToIndex(event->pos());
			m_model->setC1(index.x());
			m_model->setC2(index.y());
		}
	}
}

void ViewmdaWidget2D::mouseMoveEvent ( QMouseEvent * event ) {
		
	if (!m_model) return;
	if (m_model->polygonMode()) {
		
		if (event->buttons()&Qt::LeftButton) {
			PolygonSlices *PS=&m_model->polygonData().m_polygon_slices;
			if ((m_model->polygonData().m_polygon_fixed)&&(m_model->polygonData().m_selected_polygon_point>=0)) {
				int current_slice=m_model->currentIndex(m_model->D3());
				QPoint hold=pointToIndex(event->pos());
				QPoint pt=PS->point(current_slice,m_model->polygonData().m_selected_polygon_point);
				if (pt!=hold) {
					PS->setPoint(current_slice,m_model->polygonData().m_selected_polygon_point,hold);
					repaint();
				}
			}
		}
	}
	else {
		if ((event->buttons()&Qt::LeftButton)||(event->buttons()&Qt::RightButton)) {
			QPoint index=pointToIndex(event->pos());
			int c1=m_model->C1();
			int c2=m_model->C2();
			int diffx=qAbs(c1-index.x());
			int diffy=qAbs(c2-index.y());
			if ((diffx>0)&&(diffy>0)) {
				bool elliptical=false;
				if (event->buttons()&Qt::RightButton) 
					elliptical=true;
				QRect R(qMin(c1,index.x()),qMin(c2,index.y()),diffx,diffy);
				m_model->setSelectedRect(R,elliptical);
			}
		}
	}
}

void ViewmdaWidget2D::keyPressEvent( QKeyEvent * event ) {
	if (!m_model) return;
	int key=event->key();
	switch (key) {
		case Qt::Key_Up:
			if (event->modifiers()&Qt::ShiftModifier)
				m_model->setC3(m_model->C3()+1);
			else
				m_model->setC2(m_model->C2()-1);
			break;
		case Qt::Key_Down:
			if (event->modifiers()&Qt::ShiftModifier)
				m_model->setC3(m_model->C3()-1);
			else
				m_model->setC2(m_model->C2()+1);
			break;
		case Qt::Key_Left:
			if (event->modifiers()&Qt::ShiftModifier)
				m_model->setC3(m_model->C3()-1);
			else
				m_model->setC1(m_model->C1()-1);
			break;
		case Qt::Key_Right:
			if (event->modifiers()&Qt::ShiftModifier)
				m_model->setC3(m_model->C3()+1);
			else
				m_model->setC1(m_model->C1()+1);
			break;
	}
	event->accept();
}

void ViewmdaWidget2D::slot_windows_changed() {
	m_need_to_update_the_image=true;
	repaint();
}

