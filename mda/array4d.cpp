#include "array4d.h"
#include <stdlib.h>
#include <QString>
#include "mda.h"
#include <math.h>
#include <QStringList>


class Array4DPrivate {
public:
	float *m_data;
	int m_width;
	int m_height;
	int m_num_slices;
	int m_num_frames;
	int m_storage_order;
	
	void copy_from(const Array4D &X);
	void allocate(int W,int H,int N,int T,int storage_order);
	long get_index(int x,int y,int z,int t);
};

void Array4DPrivate::copy_from(const Array4D &X) {	
	allocate(X.width(),X.height(),X.numSlices(),X.numFrames(),X.storageOrder());
	long N=X.width()*X.height()*X.numSlices()*X.numFrames();
	for (long ii=0; ii<N; ii++)
		m_data[ii]=X.d->m_data[ii];
}
void Array4DPrivate::allocate(int W,int H,int S,int T,int storage_order) {
	m_storage_order=storage_order;
	if (m_data) free(m_data);
	if (W*H*S*T>0) {
		m_data=(float *)malloc(sizeof(float)*W*H*S*T);
		long N=W*H*S*T;
		for (long ii=0; ii<N; ii++) m_data[ii]=0;
	}
	else {
		m_data=0;
	}
	m_width=W;
	m_height=H;
	m_num_slices=S;
	m_num_frames=T;
}

Array4D::Array4D(int W,int H,int S,int T,int storage_order) {
	d=new Array4DPrivate;
	d->m_data=0;
	d->m_width=0;
	d->m_height=0;
	d->m_num_slices=0;
	d->m_num_frames=0;
	d->m_storage_order=storage_order;
	if (W*H*S*T>0) allocate(W,H,S,T,storage_order);
}
Array4D::Array4D(const Array4D &X) {
	d=new Array4DPrivate;
	d->m_data=0;
	d->m_width=0;
	d->m_height=0;
	d->m_num_slices=0;
	d->m_num_frames=0;
	d->copy_from(X);	
}
Array4D::~Array4D() {
	clear();
	delete d;
}
void Array4D::clear() {
	d->allocate(0,0,0,0,storageOrder());
}
void Array4D::allocate(int w,int h,int S,int T,int storage_order) {
	d->allocate(w,h,S,T,storage_order);
}
int Array4D::width() const {
	return d->m_width;
}
int Array4D::height() const {
	return d->m_height;
}
int Array4D::numSlices() const {
	return d->m_num_slices;
}
int Array4D::numFrames() const {
	return d->m_num_frames;
}
int Array4D::N1() const {
	return d->m_width;
}
int Array4D::N2() const {
	return d->m_height;
}
int Array4D::N3() const {
	return d->m_num_slices;
}
int Array4D::N4() const {
	return d->m_num_frames;
}
float Array4D::value(int i,int j,int k,int l) const {
	//if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)||(k<0)||(k>=d->m_num_slices)) return 0;
	if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)||(k<0)||(k>=d->m_num_slices)||(l<0)||(l>=d->m_num_frames)) {
		return 0;
	}
	long ind=d->get_index(i,j,k,l);
	return d->m_data[ind];
}
void Array4D::setValue(float val,int i,int j,int k,int l) {
	if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)||(k<0)||(k>=d->m_num_slices)||(l<0)||(l>=d->m_num_frames)) {
		return;
	}
	long ind=d->get_index(i,j,k,l);
	d->m_data[ind]=val;
}
void Array4D::incrementValue(float val,int i,int j,int k,int l) {
	if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)||(k<0)||(k>=d->m_num_slices)||(l<0)||(l>=d->m_num_frames)) {
		return;
	}
	long ind=d->get_index(i,j,k,l);
	d->m_data[ind]+=val;
}

void Array4D::setAll(float val) {
	long N=width()*height()*numSlices()*numFrames();
	for (long ii=0; ii<N; ii++)
		d->m_data[ii]=val;
}
void Array4D::operator=(const Array4D &X) {
	d->copy_from(X);
}
QList<float> Array4D::dataX(int y,int z,int t) const {
	QList<float> ret;
	for (int x=0; x<width(); x++) ret << value(x,y,z,t);
	return ret;
}
QList<float> Array4D::dataY(int x,int z,int t) const {
	QList<float> ret;
	for (int y=0; y<height(); y++) ret << value(x,y,z,t);
	return ret;
}
QList<float> Array4D::dataZ(int x,int y,int t) const {
	QList<float> ret;
	for (int z=0; z<numSlices(); z++) ret << value(x,y,z,t);
	return ret;
	
}
QList<float> Array4D::dataT(int x,int y,int z) const {
	QList<float> ret;
	for (int t=0; t<numFrames(); t++) ret << value(x,y,z,t);
	return ret;
	
}
Array3D Array4D::dataXYT(int z) const {
	Array3D ret;
	ret.allocate(width(),height(),numFrames());
	for (int t=0; t<numFrames(); t++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++)
		ret.setValue(value(x,y,z,t),x,y,t);
	return ret;
}
Array3D Array4D::dataXZT(int y) const {
	Array3D ret;
	ret.allocate(width(),numSlices(),numFrames());
	for (int t=0; t<numFrames(); t++)
	for (int z=0; z<numSlices(); z++)
	for (int x=0; x<width(); x++)
		ret.setValue(value(x,y,z,t),x,z,t);
	return ret;
}
Array3D Array4D::dataYZT(int x) const {
	Array3D ret;
	ret.allocate(height(),numSlices(),numFrames());
	for (int t=0; t<numFrames(); t++)
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
		ret.setValue(value(x,y,z,t),y,z,t);
	return ret;
}
Array3D Array4D::dataYXT(int z) const {
	Array3D ret;
	ret.allocate(height(),width(),numFrames());
	for (int t=0; t<numFrames(); t++)
	for (int x=0; x<width(); x++)
	for (int y=0; y<height(); y++)	
		ret.setValue(value(x,y,z,t),y,x,t);
	return ret;
}
Array3D Array4D::dataZXT(int y) const {
	Array3D ret;
	ret.allocate(numSlices(),width(),numFrames());
	for (int t=0; t<numFrames(); t++)
	for (int x=0; x<width(); x++)
	for (int z=0; z<numSlices(); z++)	
		ret.setValue(value(x,y,z,t),z,x,t);
	return ret;
}
Array3D Array4D::dataZYT(int x) const {
	Array3D ret;
	ret.allocate(numSlices(),height(),numFrames());
	for (int t=0; t<numFrames(); t++)
	for (int y=0; y<height(); y++)
	for (int z=0; z<numSlices(); z++)	
		ret.setValue(value(x,y,z,t),z,y,t);
	return ret;
}
Array3D Array4D::dataXYZ(int t) const {
	Array3D ret;
	ret.allocate(width(),height(),numSlices());
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++)	
		ret.setValue(value(x,y,z,t),x,y,z);
	return ret;
}
void Array4D::setDataXYZ(const Array3D &X,int t) {
	for (int z=0; z<X.numSlices(); z++)
	for (int y=0; y<X.height(); y++)
	for (int x=0; x<X.width(); x++)
		setValue(X.value(x,y,z),x,y,z,t);
}
void Array4D::setDataXY(const Array2D &X,int z,int t) {
	for (int y=0; y<X.height(); y++)
	for (int x=0; x<X.width(); x++)
		setValue(X.value(x,y),x,y,z,t);
}
float Array4D::value1(long i) const {
	return d->m_data[i];
}
void Array4D::read(QString mda_fname,int storage_order,bool read_mask) {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	printf(".");
	M.read(mda_fname.toLatin1().data());
	printf(".");
	allocate(M.size(0),M.size(1),M.size(2),M.size(3),storage_order);
	printf(".");
	for (int t=0; t<numFrames(); t++)
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		if (read_mask) { //this is a hack for importing masks for the vbb database
			if (M.mask(x,y,z,t)) setValue(0,x,y,z,t);
			else setValue(1,x,y,z,t);
		}
		else {
			setValue(M(x,y,z,t).re(),x,y,z,t);
		}
	}
	printf(".\n");
}
void Array4D::write(QString mda_fname) const {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	M.allocate(MDA_TYPE_REAL,width(),height(),numSlices(),numFrames());
	for (int t=0; t<numFrames(); t++)
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		M(x,y,z,t)=this->value(x,y,z,t);
	}
	M.write(mda_fname.toLatin1().data());
}
void Array4D::writeInt16(QString mda_fname) const {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	M.allocate(MDA_TYPE_SHORT,width(),height(),numSlices(),numFrames());
	for (int t=0; t<numFrames(); t++)
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		M(x,y,z,t)=this->value(x,y,z,t);
	}
	M.write(mda_fname.toLatin1().data());
}
void Array4D::writeByte(QString mda_fname) const {
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	M.allocate(MDA_TYPE_BYTE,width(),height(),numSlices(),numFrames());
	for (int t=0; t<numFrames(); t++)
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		M(x,y,z,t)=this->value(x,y,z,t);
	}
	M.write(mda_fname.toLatin1().data());
}

long Array4DPrivate::get_index(int x,int y,int z,int t) {
	switch (m_storage_order) {
		case STORAGE_ORDER_XYZT: return x+m_width*y+m_width*m_height*z+m_width*m_height*m_num_slices*t;
		case STORAGE_ORDER_TXYZ: return t+m_num_frames*x+m_num_frames*m_width*y+m_num_frames*m_width*m_height*z;
		default: return x+m_width*y+m_width*m_height*z+m_width*m_height*m_num_slices*t;
	}
}
int Array4D::storageOrder() const {
	return d->m_storage_order;
}
float Array4D::max() const {
	float ret=0;
	bool first=true;
	for (int t=0; t<numFrames(); t++)
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		float val=value(x,y,z,t);
		if ((first)||(val>ret)) ret=val;
		first=false;
	}
	return ret;
}
float Array4D::min() const {
	float ret=0;
	bool first=true;
	for (int t=0; t<numFrames(); t++)
	for (int z=0; z<numSlices(); z++)
	for (int y=0; y<height(); y++)
	for (int x=0; x<width(); x++) {
		float val=value(x,y,z,t);
		if ((first)||(val<ret)) ret=val;
		first=false;
	}
	return ret;
}
void Array4D::scaleBy(float val) {
	long N=d->m_width*d->m_height*d->m_num_slices*d->m_num_frames;
	for (long j=0; j<N; j++)
		d->m_data[j]*=val;
}
void Array4D::setDataZ(const QList<float> &X,int x,int y,int t) {
	for (int z=0; z<numSlices(); z++) setValue(X.value(z),x,y,z,t);
}
QColor Array4D::color(int i,int j,int k,int l) const {
	int val=(int)value(i,j,k,l);
	return QColor(val%(256*256),(val/256)%256,val/(256*256));
}
void Array4D::setColor(QColor col,int i,int j,int k,int l) {
	setValue(col.red()+col.green()*256+col.blue()*256*256,i,j,k,l);
}
void Array4D::incrementDataXYZ(const Array3D &X,int t) {
	for (int z=0; z<X.numSlices(); z++)
	for (int y=0; y<X.height(); y++)
	for (int x=0; x<X.width(); x++)
		setValue(value(x,y,z,t)+X.value(x,y,z),x,y,z,t);	
}

