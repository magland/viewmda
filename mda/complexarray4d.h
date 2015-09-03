#ifndef complexarray4d_H
#define complexarray4d_H

#include <QList>
#include "complexarray2d.h"

class ComplexArray4DPrivate;

class ComplexArray4D;
typedef ComplexArray4D ComplexArray3D;
class ComplexArray4D {
public:
	friend class ComplexArray4DPrivate;
	ComplexArray4D(int N1=0,int N2=0,int N3=0,int N4=0);
	ComplexArray4D(const ComplexArray4D &X);
	virtual ~ComplexArray4D();
	void clear();
	void allocate(int N1,int N2,int N3,int N4=1);
	int N1() const;
	int N2() const;
	int N3() const;
	int N4() const;
	COMPLEX value(int i1,int i2,int i3,int i4=0) const;
	COMPLEX value1(long i) const;
	void setValue(COMPLEX val,int i1,int i2,int i3=0,int i4=0);
	void setAll(COMPLEX val);
	void read(const QString &mda_fname);
	void write(const QString &mda_fname) const;
	void operator=(const ComplexArray4D &X);
	ComplexArray2D dataXY(int z,int t) const;
	ComplexArray3D dataXYT(int z) const;
	ComplexArray3D dataXYZ(int t) const;
	void setDataXY(const ComplexArray2D &X,int z,int t);
	void setDataXYT(const ComplexArray3D &X,int z);
	void setDataXYZ(const ComplexArray3D &X,int t);
private:
	ComplexArray4DPrivate *d;
};

#endif
