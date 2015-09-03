#ifndef array4d_H
#define array4d_H

#include <QList>
#include "array3d.h"

#define STORAGE_ORDER_XYZT 1
#define STORAGE_ORDER_TXYZ 2

class Array4DPrivate;

class Array4D {
public:
	friend class Array4DPrivate;
	Array4D(int W=0,int H=0,int S=0,int T=0,int storage_order=STORAGE_ORDER_XYZT);
	Array4D(const Array4D &X);
	virtual ~Array4D();
	void clear();
	void allocate(int W,int H,int S,int T,int storage_order=STORAGE_ORDER_XYZT);
	int width() const;
	int height() const;
	int numSlices() const;
	int numFrames() const;
	int N1() const;
	int N2() const;
	int N3() const;
	int N4() const;
	int storageOrder() const;
	float value(int i,int j,int k,int l) const;
	float value1(long i) const;
	void setValue(float val,int i,int j,int k,int l);
	void incrementValue(float val,int i,int j,int k,int l);
	void setAll(float val);
	void read(QString mda_fname,int storage_order=STORAGE_ORDER_XYZT,bool read_mask=false);
	void write(QString mda_fname) const;
	void writeInt16(QString mda_fname) const;
	void writeByte(QString mda_fname) const;
	void operator=(const Array4D &X);
	QList<float> dataX(int y,int z,int t) const;
	QList<float> dataY(int x,int z,int t) const;
	QList<float> dataZ(int x,int y,int t) const;
	QList<float> dataT(int x,int y,int z) const;
	Array3D dataXYT(int z) const;
	Array3D dataXZT(int y) const;
	Array3D dataYZT(int x) const;
	Array3D dataYXT(int z) const;
	Array3D dataZXT(int y) const;
	Array3D dataZYT(int x) const;
	Array3D dataXYZ(int t) const;
	void setDataXY(const Array2D &X,int z,int t);
	void setDataXYZ(const Array3D &X,int t);
	void setDataZ(const QList<float> &X,int x,int y,int t);
	void incrementDataXYZ(const Array3D &X,int t);
	Array4D slices(int minz,int maxz) const;
	float max() const;
	float min() const;
	void scaleBy(float val);
	QColor color(int i,int j,int k,int l) const;
	void setColor(QColor col,int i,int j,int k,int l);
	
private:
	Array4DPrivate *d;
};

#endif
