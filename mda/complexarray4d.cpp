#include "complexarray4d.h"
#include <stdlib.h>
#include <QString>
#include "mda.h"
#include <math.h>
#include <QStringList>


class ComplexArray4DPrivate {
public:
	COMPLEX *m_data;
	int m_width;
	int m_height;
	int m_num_slices;
	int m_num_frames;
	
	void copy_from(const ComplexArray4D &X);
	void allocate(int N1,int N2,int N3,int N4);
	long get_index(int x,int y,int z,int t);
};

void ComplexArray4DPrivate::copy_from(const ComplexArray4D &X) {	
	allocate(X.N1(),X.N2(),X.N3(),X.N4());
	long N=X.N1()*X.N2()*X.N3()*X.N4();
	for (long ii=0; ii<N; ii++)
		m_data[ii]=X.d->m_data[ii];
}
void ComplexArray4DPrivate::allocate(int W,int H,int S,int T) {
	if (m_data) free(m_data);
	if (W*H*S*T>0) {
		m_data=(COMPLEX *)malloc(sizeof(COMPLEX)*W*H*S*T);
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

ComplexArray4D::ComplexArray4D(int W,int H,int S,int T) {
	d=new ComplexArray4DPrivate;
	d->m_data=0;
	d->m_width=0;
	d->m_height=0;
	d->m_num_slices=0;
	d->m_num_frames=0;
	if (W*H*S*T>0) allocate(W,H,S,T);
}
ComplexArray4D::ComplexArray4D(const ComplexArray4D &X) {
	d=new ComplexArray4DPrivate;
	d->m_data=0;
	d->m_width=0;
	d->m_height=0;
	d->m_num_slices=0;
	d->m_num_frames=0;
	d->copy_from(X);	
}
ComplexArray4D::~ComplexArray4D() {
	clear();
	delete d;
}
void ComplexArray4D::clear() {
	d->allocate(0,0,0,0);
}
void ComplexArray4D::allocate(int w,int h,int S,int T) {
	d->allocate(w,h,S,T);
}
int ComplexArray4D::N1() const {
	return d->m_width;
}
int ComplexArray4D::N2() const {
	return d->m_height;
}
int ComplexArray4D::N3() const {
	return d->m_num_slices;
}
int ComplexArray4D::N4() const {
	return d->m_num_frames;
}
COMPLEX ComplexArray4D::value(int i,int j,int k,int l) const {
	if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)||(k<0)||(k>=d->m_num_slices)||(l<0)||(l>=d->m_num_frames)) {
		return 0;
	}
	long ind=d->get_index(i,j,k,l);
	return d->m_data[ind];
}
void ComplexArray4D::setValue(COMPLEX val,int i,int j,int k,int l) {
	if ((i<0)||(i>=d->m_width)||(j<0)||(j>=d->m_height)||(k<0)||(k>=d->m_num_slices)||(l<0)||(l>=d->m_num_frames)) {
		return;
	}
	long ind=d->get_index(i,j,k,l);
	d->m_data[ind]=val;
}
void ComplexArray4D::setAll(COMPLEX val) {
	long N=N1()*N2()*N3()*N4();
	for (long ii=0; ii<N; ii++)
		d->m_data[ii]=val;
}
void ComplexArray4D::operator=(const ComplexArray4D &X) {
	d->copy_from(X);
}

ComplexArray3D ComplexArray4D::dataXYT(int z) const {
	ComplexArray3D ret;
	ret.allocate(N1(),N2(),N4());
	for (int t=0; t<N4(); t++)
	for (int y=0; y<N2(); y++)
	for (int x=0; x<N1(); x++)
		ret.setValue(value(x,y,z,t),x,y,t);
	return ret;
}
ComplexArray3D ComplexArray4D::dataXYZ(int t) const {
	ComplexArray3D ret;
	ret.allocate(N1(),N2(),N3());
	for (int z=0; z<N3(); z++)
	for (int y=0; y<N2(); y++)
	for (int x=0; x<N1(); x++)	
		ret.setValue(value(x,y,z,t),x,y,z);
	return ret;
}
void ComplexArray4D::setDataXYZ(const ComplexArray3D &X,int t) {
	for (int z=0; z<X.N3(); z++)
	for (int y=0; y<X.N2(); y++)
	for (int x=0; x<X.N1(); x++)
		setValue(X.value(x,y,z),x,y,z,t);
}
void ComplexArray4D::setDataXY(const ComplexArray2D &X,int z,int t) {
	for (int y=0; y<X.N2(); y++)
	for (int x=0; x<X.N1(); x++)
		setValue(X.value(x,y),x,y,z,t);
}
COMPLEX ComplexArray4D::value1(long i) const {
	return d->m_data[i];
}
void ComplexArray4D::read(const QString &mda_fname_in) {
	QString mda_fname=mda_fname_in;
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	M.read(mda_fname.toAscii().data());
	allocate(M.size(0),M.size(1),M.size(2),M.size(3));
	for (int t=0; t<N4(); t++)
	for (int z=0; z<N3(); z++)
	for (int y=0; y<N2(); y++)
	for (int x=0; x<N1(); x++) {
		setValue(COMPLEX(M(x,y,z,t).re(),M(x,y,z,t).im()),x,y,z,t);
	}
}
void ComplexArray4D::write(const QString &mda_fname_in) const {
	QString mda_fname=mda_fname_in;
	#ifdef WIN32
	mda_fname.replace("/","\\");
	#endif
	Mda M;
	M.allocate(MDA_TYPE_COMPLEX,N1(),N2(),N3(),N4());
	for (int t=0; t<N4(); t++)
	for (int z=0; z<N3(); z++)
	for (int y=0; y<N2(); y++)
	for (int x=0; x<N1(); x++) {
		M(x,y,z,t)=MDAComplex(this->value(x,y,z,t).real(),this->value(x,y,z,t).imag());
	}
	M.write(mda_fname.toAscii().data());
}
long ComplexArray4DPrivate::get_index(int x,int y,int z,int t) {
	return x+m_width*y+m_width*m_height*z+m_width*m_height*m_num_slices*t;
}
