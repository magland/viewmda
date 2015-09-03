#include "array3d.h"
#include <stdlib.h>
#include <QString>
#include "mda.h"
#include <math.h>
#include <QStringList>

Pt3 pt3(int x,int y,int z) {
	Pt3 ret;
	ret.x=x; ret.y=y; ret.z=z;
	return ret;
}
bool operator==(const Pt3 &P1,const Pt3 &P2) {
	return ((P1.x==P2.x)&&(P1.y==P2.y)&&(P1.z==P2.z));
}
uint qHash(const Pt3 &pt) {
	return qHash(QString("%1;%2;%3").arg(pt.x).arg(pt.y).arg(pt.z));
}

Pt3f pt3f(float x,float y,float z) {
	Pt3f ret;
	ret.x=x; ret.y=y; ret.z=z;
	return ret;
}
bool operator==(const Pt3f &P1,const Pt3f &P2) {
	return ((P1.x==P2.x)&&(P1.y==P2.y)&&(P1.z==P2.z));
}
Pt3f operator+(const Pt3f &P1,const Pt3f &P2) {
	return pt3f(P1.x+P2.x,P1.y+P2.y,P1.z+P2.z);
}
Pt3f operator-(const Pt3f &P1,const Pt3f &P2) {
	return pt3f(P1.x-P2.x,P1.y-P2.y,P1.z-P2.z);
}
Pt3f operator*(const Pt3f &P,float val) {
	return pt3f(P.x*val,P.y*val,P.z*val);
}
Pt3f operator/(const Pt3f &P,float val) {
	return pt3f(P.x/val,P.y/val,P.z/val);
}
float magnitude(const Pt3f &P) {
	return sqrt(P.x*P.x+P.y*P.y+P.z*P.z);
}
Pt3f cross_product(const Pt3f &P1,const Pt3f &P2) {
	return pt3f(P1.y*P2.z-P1.z*P2.y,P1.z*P2.x-P1.x*P2.z,P1.x*P2.y-P1.y*P2.x);
}
QStringList pt3fToStringList(const Pt3f &P) {
	QStringList ret;
	ret << QString("%1").arg(P.x);
	ret << QString("%1").arg(P.y);
	ret << QString("%1").arg(P.z);
	return ret;
}
QString pt3fToString(const Pt3f &P) {
	QString ret;
	ret=QString("(%1,%2,%3)").arg(P.x).arg(P.y).arg(P.z);
	return ret;
}
Pt3f stringListToPt3f(const QStringList &L) {
	return pt3f(L.value(0).toDouble(),L.value(1).toDouble(),L.value(2).toDouble());
}

class Array3DPrivate {
public:
	float *m_data;
	int m_width;
	int m_height;
	int m_num_slices;
	int m_storage_order;
	
	void copy_from(const Array3D &X);
	void allocate(int w,int h,int N,int storage_order);
	long get_index(int x,int y,int z);
};

void Array3DPrivate::copy_from(const Array3D &X) {	
	allocate(X.width(),X.height(),X.numSlices(),X.storageOrder());
	long N=X.width()*X.height()*X.numSlices();
	for (long ii=0; ii<N; ii++)
		m_data[ii]=X.d->m_data[ii];
}
void Array3DPrivate::allocate(int w,int h,int S,int storage_order) {
	m_storage_order=storage_order;
	if (m_data) free(m_data);
	if (w*h*S>0) {
		long N=w*h*S;
		m_data=(float *)malloc(sizeof(float )*N);
		for (long ii=0; ii<N; ii++) m_data[ii]=0;
	}
	else {
		m_data=0;
	}
	m_width=w;
	m_height=h;
	m_num_slices=S;
}

Array3D::Array3D(int W,int H,int S,int storage_order) {
	d=new Array3DPrivate;
	d->m_data=0;
	d->m_width=0;
	d->m_height=0;
	d->m_num_slices=0;
	d->m_storage_order=storage_order;
	if (W*H*S>0) allocate(W,H,S,storage_order);
}
Array3D::Array3D(const Array3D &X) {
	d=new Array3DPrivate;
	d->m_data=0;
	d->m_width=0;
	d->m_height=0;
	d->m_num_slices=0;
	d->copy_from(X);	
}
Array3D::Array3D(QString mda_fname,int storage_order) {
	d=new Array3DPrivate;
	d->m_data=0;
	d->m_width=0;
	d->m_height=0;
	d->m_num_slices=0;
	d->m_storage_order=storage_order;
	read(mda_fname,storage_order);
}
Array3D::~Array3D() {
	clear();
	delete d;
}
void Array3D::clear() {
	d->allocate(0,0,0,storageOrder());
}
void Array3D::allocate(int w,int h,int N,int storage_order) {
	d->allocate(w,h,N,storage_order);
}
int Array3D::width() const {
	return d->m_width;
}
int Array3D::height() const {
	return d->m_height;
}
int Array3D::numSlices() const {
	return d->m_num_slices;
}
int Array3D::N1() const {
	return d->m_width;
}
int Array3D::N2() const {
	return d->m_height;
}
int Array3D::N3() const {
	return d->m_num_slices;
}
float Array3D::value(int i,int j,int k) const {
	//if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)||(k<0)||(k>=d->m_num_slices)) return 0;
	if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)||(k<0)||(k>=d->m_num_slices)) {
		return 0;
	}
	long ind=d->get_index(i,j,k);
	return d->m_data[ind];
}
void Array3D::setValue1(float val,long i) {
	d->m_data[i]=val;
}

void Array3D::setValue(float val,int i,int j,int k) {
	if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)||(k<0)||(k>=d->m_num_slices)) {
		return;
	}
	long ind=d->get_index(i,j,k);
	d->m_data[ind]=val;
}
void Array3D::incrementValue(float val,int i,int j,int k) {
	if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)||(k<0)||(k>=d->m_num_slices)) {
		return;
	}
	long ind=d->get_index(i,j,k);
	d->m_data[ind]+=val;
}

void Array3D::setAll(float val) {
	long N=width()*height()*numSlices();
	for (long ii=0; ii<N; ii++)
		d->m_data[ii]=val;
}
void Array3D::operator=(const Array3D &X) {
	d->copy_from(X);
}
QList<float > Array3D::dataX(int y,int z) const {
	QList<float > ret;
	for (int x=0; x<width(); x++) ret << value(x,y,z);
	return ret;
}
QList<float > Array3D::dataY(int x,int z) const {
	QList<float > ret;
	for (int y=0; y<height(); y++) ret << value(x,y,z);
	return ret;
}
QList<float > Array3D::dataZ(int x,int y) const {
	QList<float > ret;
	for (int z=0; z<numSlices(); z++) ret << value(x,y,z);
	return ret;
	
}
Array2D Array3D::dataXY(int z) const {
	Array2D ret;
	ret.allocate(width(),height());
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++)
		ret.setValue(value(x,y,z),x,y);
	return ret;
}
Array2D Array3D::dataXZ(int y) const {
	Array2D ret;
	ret.allocate(width(),numSlices());
	for (int z=0; z<numSlices(); z++)
	for (int x=0; x<width(); x++)
		ret.setValue(value(x,y,z),x,z);
	return ret;
}
Array2D Array3D::dataYZ(int x) const {
	Array2D ret;
	ret.allocate(height(),numSlices());
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
		ret.setValue(value(x,y,z),y,z);
	return ret;
}
Array2D Array3D::dataYX(int z) const {
	Array2D ret;
	ret.allocate(height(),width());
	for (int x=0; x<width(); x++)
	for (int y=0; y<height(); y++)	
		ret.setValue(value(x,y,z),y,x);
	return ret;
}
Array2D Array3D::dataZX(int y) const {
	Array2D ret;
	ret.allocate(numSlices(),width());
	for (int x=0; x<width(); x++)
	for (int z=0; z<numSlices(); z++)	
		ret.setValue(value(x,y,z),z,x);
	return ret;
}
Array2D Array3D::dataZY(int x) const {
	Array2D ret;
	ret.allocate(numSlices(),height());
	for (int y=0; y<height(); y++)
	for (int z=0; z<numSlices(); z++)	
		ret.setValue(value(x,y,z),z,y);
	return ret;
}
void Array3D::setDataXY(const Array2D &X,int z) {
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++)
		setValue(X.value(x,y),x,y,z);
}
void Array3D::setDataX(const QList<float > &X,int y,int z) {
	for (int x=0; x<X.count(); x++)
		setValue(X[x],x,y,z);
}
void Array3D::setDataY(const QList<float > &X,int x,int z) {
	for (int y=0; y<X.count(); y++)
		setValue(X[y],x,y,z);
}
void Array3D::setDataZ(const QList<float > &X,int x,int y) {
	for (int z=0; z<X.count(); z++)
		setValue(X[z],x,y,z);
}
float Array3D::value1(long i) const {
	return d->m_data[i];
}
void Array3D::read(QString mda_fname,int storage_order) {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	printf(".");
	M.read(mda_fname.toLatin1().data());
	printf(".");
	allocate(M.size(0),M.size(1),M.size(2),storage_order);
	printf(".");
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		setValue(M(x,y,z).re(),x,y,z);
	}
	printf(".\n");
}
void Array3D::readSlices(QString mda_fname,int minslice,int maxslice,int storage_order) {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	printf(".");
	M.readSlices(mda_fname.toLatin1().data(),minslice,maxslice);
	printf(".");
	allocate(M.size(0),M.size(1),M.size(2),storage_order);
	printf(".");
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		setValue(M(x,y,z).re(),x,y,z);
	}
	printf(".\n");
}

void Array3D::write(QString mda_fname) const {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	M.allocate(MDA_TYPE_REAL,width(),height(),numSlices());
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		M(x,y,z)=this->value(x,y,z);
	}
	M.write(mda_fname.toLatin1().data());
}
void Array3D::writeInt16(QString mda_fname) const {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	M.allocate(MDA_TYPE_SHORT,width(),height(),numSlices());
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		M(x,y,z)=this->value(x,y,z);
	}
	M.write(mda_fname.toLatin1().data());
}
void Array3D::writeUint16(QString mda_fname) const {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	M.allocate(MDA_TYPE_UINT16,width(),height(),numSlices());
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		float val=this->value(x,y,z);
		if (val<0) val=0;
		M(x,y,z)=val;
	}
	M.write(mda_fname.toLatin1().data());
}

long Array3DPrivate::get_index(int x,int y,int z) {
	switch (m_storage_order) {
		case STORAGE_ORDER_XYZ: return x+m_width*y+m_width*m_height*z;
		case STORAGE_ORDER_XZY: return x+m_width*z+m_width*m_num_slices*y;
		case STORAGE_ORDER_YXZ: return y+m_height*x+m_height*m_width*z;
		case STORAGE_ORDER_YZX: return y+m_height*z+m_height*m_num_slices*x;
		case STORAGE_ORDER_ZXY: return z+m_num_slices*x+m_num_slices*m_width*y;
		case STORAGE_ORDER_ZYX: return z+m_num_slices*y+m_num_slices*m_height*x;
		default: return x+m_width*y+m_width*m_height*z;
	}
}
int Array3D::storageOrder() const {
	return d->m_storage_order;
}
float Array3D::max() const {
	float ret=0;
	bool first=true;
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		float val=value(x,y,z);
		if ((first)||(val>ret)) ret=val;
		first=false;
	}
	return ret;
}
float Array3D::min() const {
	float ret=0;
	bool first=true;
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		float val=value(x,y,z);
		if ((first)||(val<ret)) ret=val;
		first=false;
	}
	return ret;
}
void Array3D::scaleBy(float val) {
	long N=d->m_width*d->m_height*d->m_num_slices;
	for (long j=0; j<N; j++)
		d->m_data[j]*=val;
}
Array3D Array3D::slices(int minz,int maxz) const {
	Array3D ret(width(),height(),maxz-minz+1);
	for (int z=minz; z<=maxz; z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		ret.setValue(value(x,y,z),x,y,z-minz);
	}
	return ret;
}
float *Array3D::ptrX(int y,int z)  { //must be storage order XYZ or XZY
	if (d->m_storage_order==STORAGE_ORDER_XYZ) 
		return &d->m_data[y*width()+z*width()*height()];
	else if (d->m_storage_order==STORAGE_ORDER_XYZ) 
		return &d->m_data[z*width()+y*width()*numSlices()];
	else 
		return 0;
}
float *Array3D::ptrZ(int x,int y)  { //must be storage order ZXY or ZYX
	if (d->m_storage_order==STORAGE_ORDER_ZXY) 
		return &d->m_data[x*numSlices()+y*numSlices()*width()];
	else if (d->m_storage_order==STORAGE_ORDER_ZYX) 
		return &d->m_data[y*numSlices()+x*numSlices()*height()];
	else 
		return 0;
}
Mda Array3D::toMda() const {
	Mda ret;
	ret.allocate(MDA_TYPE_REAL,width(),height(),numSlices());
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		ret(x,y,z)=value(x,y,z);
	}	
	return ret;
}
Array3D Array3D::slices(const QList<int> &indices) const {
	Array3D ret(width(),height(),indices.count(),d->m_storage_order);
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		for (int z=0; z<indices.count(); z++)
			ret.setValue(value(x,y,indices[z]),x,y,z);
	}
	return ret;
}
float *Array3D::data1() const {
	return d->m_data;
}
QColor Array3D::color(int i,int j,int k) const {
	int val=(int)value(i,j,k);
	return QColor(val%(256*256),(val/256)%256,val/(256*256));
}
void Array3D::setColor(QColor col,int i,int j,int k) {
	setValue(col.red()+col.green()*256+col.blue()*256*256,i,j,k);
}
void Array3D::getData(float *ret) const {
	long N=d->m_width*d->m_height*d->m_num_slices;
	for (long ii=0; ii<N; ii++)
		ret[ii]=d->m_data[ii];
}
void Array3D::setData(float *D) {
	long N=d->m_width*d->m_height*d->m_num_slices;
	for (long ii=0; ii<N; ii++)
		d->m_data[ii]=D[ii];	
}


