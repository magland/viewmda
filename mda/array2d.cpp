#include "array2d.h"
#include <stdlib.h>
#include "mda.h"
#include <QThread>
#include <QMutex>

class Array2DPrivate {
public:
	float *m_data;
	int m_width;
	int m_height;
	
	void copy_from(const Array2D &X);
	void allocate(int w,int h);
};



void Array2DPrivate::copy_from(const Array2D &X) {	
	allocate(X.width(),X.height());
	long N=X.width()*X.height();
	for (long ii=0; ii<N; ii++)
		m_data[ii]=X.d->m_data[ii];
}
void Array2DPrivate::allocate(int w,int h) {
	if (m_data) free(m_data);
	if (w*h>0) {
		m_data=(float *)malloc(sizeof(float)*w*h);
		long N=w*h;
		for (long ii=0; ii<N; ii++) m_data[ii]=0;
	}
	else {
		m_data=0;
	}
	m_width=w;
	m_height=h;
}

Array2D::Array2D(int W,int H) {
	d=new Array2DPrivate;
	d->m_data=0;
	d->m_width=0;
	d->m_height=0;
	if (W*H>0) allocate(W,H);
}
Array2D::Array2D(const Array2D &X) {
	d=new Array2DPrivate;
	d->m_data=0;
	d->m_width=0;
	d->m_height=0;
	d->copy_from(X);	
}
Array2D::Array2D(const QString &mda_fname) {
	d=new Array2DPrivate;
	d->m_data=0;
	d->m_width=0;
	d->m_height=0;
	read(mda_fname);
}
Array2D::~Array2D() {
	clear();
	delete d;
}
void Array2D::clear() {
	d->allocate(0,0);
}
void Array2D::allocate(int w,int h) {
	d->allocate(w,h);
}
int Array2D::width() const {
	return d->m_width;
}
int Array2D::height() const {
	return d->m_height;
}
int Array2D::N1() const {
	return d->m_width;
}
int Array2D::N2() const {
	return d->m_height;
}
float Array2D::value(int i,int j) const {
	if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)) return 0;
	return d->m_data[i+width()*j];
}
float Array2D::getValue(int i,int j) const {
	return value(i,j);
}
void Array2D::setValue(float val,int i,int j) {
	if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)) return;
	d->m_data[i+width()*j]=val;
}
void Array2D::incrementValue(float val,int i,int j) {
	if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)) return;
	d->m_data[i+width()*j]+=val;
}
void Array2D::setAll(float val) {
	long N=width()*height();
	for (long ii=0; ii<N; ii++)
		d->m_data[ii]=val;
}
void Array2D::operator=(const Array2D &X) {
	d->copy_from(X);
}
void Array2D::getData(float *ret) const {
	long N=d->m_width*d->m_height;
	for (long ii=0; ii<N; ii++)
		ret[ii]=d->m_data[ii];
}
void Array2D::setData(float *D) {
	long N=d->m_width*d->m_height;
	for (long ii=0; ii<N; ii++)
		d->m_data[ii]=D[ii];
}
float Array2D::value1(long i) const {
	return d->m_data[i];
}
void Array2D::setValue1(float val,long i) const {
	d->m_data[i]=val;
}
void Array2D::read(QString mda_fname) {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	M.read(mda_fname.toLatin1().data());
	allocate(M.size(0),M.size(1));
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		setValue(M(x,y).re(),x,y);
	}
}
void Array2D::readSlice(QString mda_fname,int slice_number) {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	M.readSlices(mda_fname.toLatin1().data(),slice_number,slice_number);
	allocate(M.size(0),M.size(1));
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		setValue(M(x,y).re(),x,y);
	}
}
void Array2D::write(QString mda_fname) const {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	M.allocate(MDA_TYPE_REAL,width(),height());
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		M(x,y)=this->value(x,y);
	}
	M.write(mda_fname.toLatin1().data());
}
void write_mda(const QString &fname2,const QList<float> &data) {
	QString fname=fname2;
	#ifdef WIN32
	fname.replace("/","\\");
	#endif
	Mda M;
	M.allocate(MDA_TYPE_REAL,data.count(),1);
	for (int x=0; x<data.count(); x++) {
		M(x)=data[x];
	}
	M.write(fname.toLatin1().data());
}
void read_mda(const QString &fname2,QList<float> &data) {
	QString fname=fname2;
	data.clear();
	#ifdef WIN32
	fname.replace("/","\\");
	#endif
	Mda M;
	M.read(fname.toLatin1().data());
	for (int x=0; x<M.size(0); x++) {
		data << M(x).re();
	}
}
/*QList<float> read_float_list(const QString &fname) {
	QList<float> X;
	read_mda(fname,X);
	return X;
}*/
float Array2D::max() const {
	float ret=0;
	bool first=true;
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		float val=value(x,y);
		if ((first)||(val>ret)) ret=val;
		first=false;
	}
	return ret;
}
float Array2D::min() const {
	float ret=0;
	bool first=true;
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		float val=value(x,y);
		if ((first)||(val<ret)) ret=val;
		first=false;
	}
	return ret;
}
QList<float> Array2D::dataY(int x) const {
	QList<float> ret;
	for (int y=0; y<d->m_height; y++)
		ret << value(x,y);
	return ret;
}
QList<float> Array2D::dataX(int y) const {
	QList<float> ret;
	for (int x=0; x<d->m_width; x++)
		ret << value(x,y);
	return ret;
}
void Array2D::setDataX(const QList<float> &X,int y) {
	for (int x=0; x<width(); x++)
		setValue(X.value(x),x,y);
}
void Array2D::setDataY(const QList<float> &X,int x) {
	for (int y=0; y<height(); y++)
		setValue(X.value(y),x,y);
}


void Array2D::scaleBy(float val) {
	long N=d->m_width*d->m_height;
	for (int j=0; j<N; j++)
		d->m_data[j]*=val;
}
void Array2D::add(const Array2D &X) {
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		setValue(value(x,y)+X.value(x,y),x,y);
	}
}
void add(QList<float> &X,const QList<float> &Y) {
	for (int i=0; i<X.count(); i++)
		X[i]+=Y[i];
}
Array2D Array2D::transpose() const {
	Array2D ret(height(),width()); 
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		ret.setValue(value(x,y),y,x);
	}
	return ret;
}
float *Array2D::data1() const {
	return d->m_data;
}
float *Array2D::ptrX(int y) const {
	return &d->m_data[y*width()];
}

void copy_array(float *dst,float *src,long N) {
	memcpy(dst,src,N*sizeof(float));
}
class IpThread : public QThread {
public:
	QMutex m_mutex;
	float *X1;
	float *X2;
	long N;
	double result;
	bool m_finished;
	IpThread() {
		m_finished=false;
	}
	void run() {
		m_mutex.lock();
		result=ip(X1,X2,N);
		m_finished=true;
		m_mutex.unlock();
	}
};
double ip(float *X1,float *X2,long N) {
	double ret=0;
	for (long i=0; i<N; i++) ret+=X1[i]*X2[i];
	return ret;
}
void normalize(float *X,long N) {
	double norm=sqrt(ip(X,X,N));
	if (norm) {
		for (long j=0; j<N; j++) X[j]/=norm;
	}
}
void normalize(QList<float> &X) {
	double norm=0;
	for (int i=0; i<X.count(); i++) norm+=X[i]*X[i];
	norm=sqrt(norm);
	if (norm) {
		for (long j=0; j<X.count(); j++) X[j]/=norm;
	}
}

Mda Array2D::toMda() const {
	Mda ret;
	ret.allocate(MDA_TYPE_REAL,width(),height());
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		ret(x,y)=value(x,y);
	}	
	return ret;
}
Array2D matrix_multiply(const Array2D &M1,const Array2D &M2) {
	if (M1.height()!=M2.width()) return Array2D();
	Array2D ret(M1.width(),M2.height());
	for (int j=0; j<ret.height(); j++)
	for (int i=0; i<ret.width(); i++) {
		double val=0;
		for (int k=0; k<M1.height(); k++) {
			val+=M1.value(i,k)*M2.value(k,j);
		}
		ret.setValue(val,i,j);
	}
	return ret;
}
QColor Array2D::color(int i,int j) const {
	int val=(int)value(i,j);
	return QColor(val%(256*256),(val/256)%256,val/(256*256));
}
void Array2D::setColor(QColor col,int i,int j) {
	setValue(col.red()+col.green()*256+col.blue()*256*256,i,j);
}
