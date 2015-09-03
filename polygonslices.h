#ifndef polygonslices_H
#define polygonslices_H
#include <QPoint>
#include <QList>

struct PolygonSlicesAnchor {
	int slice_index;
	int point_index;
	QPoint pt;
};

class PolygonSlices {
	
public:
	PolygonSlices();
	virtual ~PolygonSlices();
	
	void setSliceCount(int val) {m_num_slices=val;}
	int sliceCount() {return m_num_slices;}
	int pointCount() {return m_num_points;}
	QPoint point(int slice_index,int point_index);
	bool isAnchor(int slice_index,int point_index);
	void setPoint(int slice_index,int point_index,QPoint pt);
	void insertPoint(int point_index);
	void insertPoint(int slice_index,int point_index,QPoint pt);
	void clear() {m_num_points=0; m_anchors.clear();}
	
private:
	int m_num_slices;
	int m_num_points;
	QList<PolygonSlicesAnchor> m_anchors;
};

#endif
