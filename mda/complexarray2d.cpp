#include "complexarray2d.h"
#include <stdlib.h>
#include "mda.h"
#include <QThread>
#include <QMutex>

class ComplexArray2DPrivate {
public:
	COMPLEX *m_data;
	int m_width;
	int m_height;
	
	void copy_from(const ComplexArray2D &X);
	void allocate(int w,int h);
};



void ComplexArray2DPrivate::copy_from(const ComplexArray2D &X) {	
	allocate(X.N1(),X.N2());
	long N=X.N1()*X.N2();
	for (long ii=0; ii<N; ii++)
		m_data[ii]=X.d->m_data[ii];
}
void ComplexArray2DPrivate::allocate(int w,int h) {
	if (m_data) free(m_data);
	if (w*h>0) {
		m_data=(COMPLEX *)malloc(sizeof(COMPLEX)*w*h);
		long N=w*h;
		for (long ii=0; ii<N; ii++) m_data[ii]=0;
	}
	else {
		m_data=0;
	}
	m_width=w;
	m_height=h;
}

ComplexArray2D::ComplexArray2D(int W,int H) {
	d=new ComplexArray2DPrivate;
	d->m_data=0;
	d->m_width=0;
	d->m_height=0;
	if (W*H>0) allocate(W,H);
}
ComplexArray2D::ComplexArray2D(const ComplexArray2D &X) {
	d=new ComplexArray2DPrivate;
	d->m_data=0;
	d->m_width=0;
	d->m_height=0;
	d->copy_from(X);	
}
ComplexArray2D::ComplexArray2D(const QString &mda_fname) {
	d=new ComplexArray2DPrivate;
	d->m_data=0;
	d->m_width=0;
	d->m_height=0;
	read(mda_fname);
}
ComplexArray2D::~ComplexArray2D() {
	clear();
	delete d;
}
void ComplexArray2D::clear() {
	d->allocate(0,0);
}
void ComplexArray2D::allocate(int w,int h) {
	d->allocate(w,h);
}
int ComplexArray2D::N1() const {
	return d->m_width;
}
int ComplexArray2D::N2() const {
	return d->m_height;
}
COMPLEX ComplexArray2D::value(int i,int j) const {
	if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)) return 0;
	return d->m_data[i+N1()*j];
}
COMPLEX ComplexArray2D::getValue(int i,int j) const {
	return value(i,j);
}
void ComplexArray2D::setValue(COMPLEX val,int i,int j) {
	if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)) return;
	d->m_data[i+N1()*j]=val;
}
void ComplexArray2D::setAll(COMPLEX val) {
	long N=N1()*N2();
	for (long ii=0; ii<N; ii++)
		d->m_data[ii]=val;
}
void ComplexArray2D::operator=(const ComplexArray2D &X) {
	d->copy_from(X);
}
void ComplexArray2D::getData(COMPLEX *ret) const {
	long N=d->m_width*d->m_height;
	for (long ii=0; ii<N; ii++)
		ret[ii]=d->m_data[ii];
}
void ComplexArray2D::setData(COMPLEX *D) {
	long N=d->m_width*d->m_height;
	for (long ii=0; ii<N; ii++)
		d->m_data[ii]=D[ii];
}
COMPLEX ComplexArray2D::value1(long i) const {
	return d->m_data[i];
}
void ComplexArray2D::setValue1(COMPLEX val,long i) const {
	d->m_data[i]=val;
}
void ComplexArray2D::read(QString mda_fname) {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	M.read(mda_fname.toAscii().data());
	allocate(M.size(0),M.size(1));
	for (int y=0; y<N2(); y++)
	for (int x=0; x<N1(); x++) {
		COMPLEX tmp(M(x,y).re(),M(x,y).im());
		setValue(tmp,x,y);
	}
}
void ComplexArray2D::readSlice(QString mda_fname,int slice_number) {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	M.readSlices(mda_fname.toAscii().data(),slice_number,slice_number);
	allocate(M.size(0),M.size(1));
	for (int y=0; y<N2(); y++)
	for (int x=0; x<N1(); x++) {
		COMPLEX tmp(M(x,y).re(),M(x,y).im());
		setValue(tmp,x,y);
	}
}
void ComplexArray2D::write(QString mda_fname) const {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	M.allocate(MDA_TYPE_REAL,N1(),N2());
	for (int y=0; y<N2(); y++)
	for (int x=0; x<N1(); x++) {
		COMPLEX tmp=this->value(x,y);
		M(x,y)=MDAComplex(tmp.real(),tmp.imag());
	}
	M.write(mda_fname.toAscii().data());
}
COMPLEX *ComplexArray2D::data1() const {
	return d->m_data;
}


