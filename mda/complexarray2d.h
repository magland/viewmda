#ifndef complexarray2d_H
#define complexarray2d_H

#include <QString>
#include <QList>
#include <QDebug>
#include <QColor>
#include <math.h>
#include "mda.h"
#include <cmath>
#include <complex>

typedef std::complex<float> COMPLEX;

class ComplexArray2DPrivate;

class ComplexArray2D {
public:
	friend class ComplexArray2DPrivate;
	ComplexArray2D(int W=0,int H=0);
	ComplexArray2D(const ComplexArray2D &X);
	ComplexArray2D(const QString &mda_fname);
	virtual ~ComplexArray2D();
	void clear();
	void allocate(int N1,int N2=1);
	int N1() const;
	int N2() const;
	COMPLEX getValue(int i,int j=0) const;
	COMPLEX value(int i,int j=0) const;
	COMPLEX value1(long i) const;
	void setValue1(COMPLEX val,long i) const;
	void setValue(COMPLEX val,int i,int j=0);
	void setAll(COMPLEX val);
	void getData(COMPLEX *ret) const;
	void setData(COMPLEX *D);
	void read(QString mda_fname);
	void write(QString mda_fname) const;
	void readSlice(QString mda_fname,int slice_number);
	void operator=(const ComplexArray2D &X);

	
	COMPLEX *data1() const;
private:
	ComplexArray2DPrivate *d;
};

class ComplexArray1D : public ComplexArray2D {
public:
	ComplexArray1D() : ComplexArray2D(0,0) {}
	ComplexArray1D(int N) : ComplexArray2D(N,1) {}
};

#endif

