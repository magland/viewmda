#include "polygonslices.h"
#include <QSet>
#include <QDebug>

PolygonSlices::PolygonSlices() 
{
	m_num_slices=1;
	m_num_points=0;
}

PolygonSlices::~PolygonSlices()
{
}

QPoint PolygonSlices::point(int slice_index,int point_index) {
	PolygonSlicesAnchor A1; bool A1_found=false; A1.slice_index=-1;
	PolygonSlicesAnchor A2; bool A2_found=false; A2.slice_index=m_num_slices;
	foreach (PolygonSlicesAnchor A,m_anchors) {
		if ((A.point_index==point_index)&&(A.slice_index<=slice_index)&&(A.slice_index>A1.slice_index)) {
			A1_found=true;
			A1=A;
		}
		else if ((A.point_index==point_index)&&(A.slice_index>=slice_index)&&(A.slice_index<A2.slice_index)) {
			A2_found=true;
			A2=A;
		}
	}
	if (A1.slice_index==slice_index) return A1.pt;
	if (A2.slice_index==slice_index) return A2.pt;
	if ((A1_found)&&(A2_found)) {
		double P=((double)slice_index-A1.slice_index)/(A2.slice_index-A1.slice_index);
		int x=(int)(A1.pt.x()+P*(A2.pt.x()-A1.pt.x()));
		int y=(int)(A1.pt.y()+P*(A2.pt.y()-A1.pt.y()));
		return QPoint(x,y);
	}
	else if (A1_found) return A1.pt;
	else if (A2_found) return A2.pt;
	else return QPoint(0,0);
}
bool PolygonSlices::isAnchor(int slice_index,int point_index) {
	foreach (PolygonSlicesAnchor A,m_anchors) {
		if ((A.slice_index==slice_index)&&(A.point_index==point_index))
			return true;
	}
	return false;
}
void PolygonSlices::setPoint(int slice_index,int point_index,QPoint pt) {
	if (slice_index<0) return;
	if (slice_index>=m_num_slices) return;
	if (point_index<0) return;
	if (point_index>=m_num_points) return;
	for (int j=0; j<m_anchors.count(); j++) {
		if ((m_anchors[j].slice_index==slice_index)&&(m_anchors[j].point_index==point_index)) {
			m_anchors[j].pt=pt;
			return;
		}
	}
	PolygonSlicesAnchor A;
	A.slice_index=slice_index;
	A.point_index=point_index;
	A.pt=pt;
	m_anchors << A;
}
void PolygonSlices::insertPoint(int point_index) {
	if (point_index<m_num_points) {
		for (int j=0; j<m_anchors.count(); j++) {
			if (m_anchors[j].point_index>=point_index) {
				m_anchors[j].point_index++;
			}
		}	
	}
	m_num_points++;
	int before_index=point_index-1; if (before_index<0) before_index=m_num_points-1;
	int after_index=point_index+1; if (after_index>=m_num_points) after_index=0;
	
	int num_anchors=m_anchors.count(); 
	QSet<int> slices_with_anchors;
	for (int j=0; j<num_anchors; j++) {
		if ((m_anchors[j].point_index==before_index)||(m_anchors[j].point_index==after_index)) {
			slices_with_anchors.insert(m_anchors[j].slice_index);
		}
	}
	foreach (int si, slices_with_anchors) {
		PolygonSlicesAnchor A;
		A.slice_index=si;
		A.point_index=point_index;
		A.pt=(point(si,before_index)+point(si,after_index))/2;
		m_anchors << A;
	}
}

void PolygonSlices::insertPoint(int slice_index,int point_index,QPoint pt) {
	if (point_index<m_num_points) {
		for (int j=0; j<m_anchors.count(); j++) {
			if (m_anchors[j].point_index>=point_index) {
				m_anchors[j].point_index++;
			}
		}	
	}
	m_num_points++;
	
	PolygonSlicesAnchor A;
	A.slice_index=slice_index;
	A.point_index=point_index;
	A.pt=pt;
	m_anchors << A;
}

