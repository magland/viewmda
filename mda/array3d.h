#ifndef array3d_H
#define array3d_H

#include <QList>
#include "array2d.h"

#define STORAGE_ORDER_XYZ 1
#define STORAGE_ORDER_XZY 2
#define STORAGE_ORDER_YXZ 3
#define STORAGE_ORDER_YZX 4
#define STORAGE_ORDER_ZXY 5
#define STORAGE_ORDER_ZYX 6

class Array3DPrivate;

struct Pt3 {
	int x,y,z;
};
Pt3 pt3(int x,int y,int z);
bool operator==(const Pt3 &P1,const Pt3 &P2);
uint qHash(const Pt3 &pt);

struct Pt3f {
	float x,y,z;
};
Pt3f pt3f(float x,float y,float z);
bool operator==(const Pt3f &P1,const Pt3f &P2);
Pt3f operator+(const Pt3f &P1,const Pt3f &P2);
Pt3f operator-(const Pt3f &P1,const Pt3f &P2);
Pt3f operator*(const Pt3f &P,float val);
Pt3f operator/(const Pt3f &P,float val);
float magnitude(const Pt3f &P);
Pt3f cross_product(const Pt3f &P1,const Pt3f &P2);
QStringList pt3fToStringList(const Pt3f &P);
Pt3f stringListToPt3f(const QStringList &L);
QString pt3fToString(const Pt3f &P);

class Array3D {
public:
	friend class Array3DPrivate;
	Array3D(int W=0,int H=0,int S=0,int storage_order=STORAGE_ORDER_XYZ);
	Array3D(const Array3D &X);
	Array3D(QString mda_fname,int storage_order=STORAGE_ORDER_XYZ);
	virtual ~Array3D();
	void clear();
	void allocate(int w,int h,int N,int storage_order=STORAGE_ORDER_XYZ);
	int width() const;
	int height() const;
	int numSlices() const;
	int N1() const;
	int N2() const;
	int N3() const;
	int storageOrder() const;
	float value(int i,int j,int k) const;
	float value1(long i) const;
	void setValue(float val,int i,int j,int k);
	void setValue1(float val,long i);
	void incrementValue(float val,int i,int j,int k);
	void setAll(float val);
	void read(QString mda_fname,int storage_order=STORAGE_ORDER_XYZ);
	void readSlices(QString mda_fname,int minslice=-1,int maxslice=-1,int storage_order=STORAGE_ORDER_XYZ);
	void write(QString mda_fname) const;
	void writeInt16(QString mda_fname) const;
	void writeUint16(QString mda_fname) const;
	void operator=(const Array3D &X);
	QList<float> dataX(int y,int z) const;
	QList<float> dataY(int x,int z) const;
	QList<float> dataZ(int x,int y) const;
	float *ptrX(int y,int z); //must be storage order XYZ or XZY
	float *ptrZ(int x,int y); //must be storage order ZXY or ZYX
	Array2D dataXY(int z) const;
	Array2D dataXZ(int y) const;
	Array2D dataYZ(int x) const;
	Array2D dataYX(int z) const;
	Array2D dataZX(int y) const;
	Array2D dataZY(int x) const;
	void setDataXY(const Array2D &X,int z);
	void setDataX(const QList<float> &X,int y,int z);
	void setDataY(const QList<float> &X,int x,int z);
	void setDataZ(const QList<float> &X,int x,int y);
	Array3D slices(int minz,int maxz) const;
	Array3D slices(const QList<int> &indices) const;
	float max() const;
	float min() const;
	void scaleBy(float val);
	QColor color(int i,int j,int k) const;
	void setColor(QColor col,int i,int j,int k);
	void getData(float *ret) const;
	void setData(float *D);
	
	float *data1() const;
	
	Mda toMda() const;
	
private:
	Array3DPrivate *d;
};

#endif
