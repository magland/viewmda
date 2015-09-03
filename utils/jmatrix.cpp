#include "jmatrix.h"
#ifndef Q_OS_MAC 
#include <malloc.h>
#endif
#include <QtGlobal>

class JMatrixPrivate {
public:
	JMatrix *q;
	int m_M,m_N;
	double *m_data;
	
	void swaprows(int r1,int r2);
	void scalerow(int r,double val);
	void rowop(int r1,int r2,double val);
};

JMatrix::JMatrix(int M,int N) 
{
	d=new JMatrixPrivate;
	d->q=this;
	d->m_data=0;
	d->m_M=0;
	d->m_N=0;
	allocate(M,N);
}
JMatrix::JMatrix(const JMatrix &M) {
	d=new JMatrixPrivate;
	d->q=this;
	d->m_data=0;
	d->m_M=0;
	d->m_N=0;
	(*this)=M;
}
JMatrix::~JMatrix()
{
	allocate(0,0);
	delete d;
}
void JMatrix::allocate(int M,int N) {
	if (d->m_data) free(d->m_data);
	d->m_data=0;
	d->m_M=0;
	d->m_N=0;
	if (M*N>0) {
		d->m_data=(double *)malloc(sizeof(double)*M*N);
		d->m_M=M;
		d->m_N=N;
		for (int i=0; i<M*N; i++)
			d->m_data[i]=0;
	}
}

void JMatrix::operator=(const JMatrix &M) {
	allocate(M.M(),M.N());
	for (int i=0; i<d->m_M*d->m_N; i++) {
		d->m_data[i]=M.d->m_data[i];
	}
}
int JMatrix::M() const {
	return d->m_M;
}
int JMatrix::N() const {
	return d->m_N;
}
double JMatrix::value(int i,int j) const {
	if (i>=d->m_M) return 0;
	if (j>=d->m_N) return 0;
	return d->m_data[i+d->m_M*j];
}
void JMatrix::setValue(double val,int i,int j) {
	if (i>=d->m_M) return;
	if (j>=d->m_N) return;
	d->m_data[i+d->m_M*j]=val;
}
JMatrix JMatrix::operator*(const JMatrix &M) {
	JMatrix ret;
	if (d->m_N!=M.M()) return ret;
	ret.allocate(d->m_M,M.N());
	for (int i=0; i<d->m_M; i++)
	for (int j=0; j<M.N(); j++) {
		double val=0;
		for (int k=0; k<d->m_N; k++) {
			val+=value(i,k)*M.value(k,j);
		}
		ret.setValue(val,i,j);
	}
	return ret;
}
JMatrix JMatrix::inverse() const {
	JMatrix ret;
	if (M()!=N()) return ret;
	
	JMatrix hold;
	
	ret.allocate(N(),N());

	//set identity
	for (int j=0; j<N(); j++)
		ret.setValue(1,j,j);
	hold=(*this);

	for (int j=0; j<N(); j++) {
		long maxind=j;
		double maxval=0;
		for (int k=j; k<M(); k++) {
			if (qAbs(hold.value(k,j))>maxval) {
				maxval=qAbs(hold.value(k,j));
				maxind=j;
			}
		}
		if (maxind!=j) {
			hold.d->swaprows(j,maxind);
			ret.d->swaprows(j,maxind);
		}
		double val=hold.value(j,j);
		hold.d->scalerow(j,1.0/val);
		ret.d->scalerow(j,1.0/val);

		for (int k=0; k<M(); k++) {
			if (k!=j) {
				val=hold.value(k,j);
				hold.d->rowop(j,k,val*(-1));
				ret.d->rowop(j,k,val*(-1));
			}
		}
	}

	return ret;	
}

void JMatrixPrivate::swaprows(int r1,int r2) {
	for (int j=0; j<m_N; j++) {
		double hold=q->value(r1,j);
		q->setValue(q->value(r2,j),r1,j);
		q->setValue(hold,r2,j);
	}
}
void JMatrixPrivate::scalerow(int r,double val) {
	for (int j=0; j<m_N; j++) {
		q->setValue(q->value(r,j)*val,r,j);
	}
}
void JMatrixPrivate::rowop(int r1,int r2,double val) {
	for (int j=0; j<m_N; j++) {
		q->setValue(q->value(r2,j)+q->value(r1,j)*val,r2,j);
	}
}



#ifdef USING_ARRAY3D
#include "array3d.h"
void JMatrix::write(QString fname) {
	Array2D X(d->m_M,d->m_N);
	for (int j=0; j<d->m_N; j++)
	for (int i=0; i<d->m_M; i++) {
		X.setValue(value(i,j),i,j);
	}
	X.write(fname);
}
void JMatrix::read(QString fname) {
	Array2D X;
	X.read(fname);
	allocate(X.width(),X.height());
	for (int j=0; j<d->m_N; j++)
	for (int i=0; i<d->m_M; i++) {
		setValue(X.value(i,j),i,j);
	}
}
#endif
JMatrix JMatrix::transpose() const {
	JMatrix ret(d->m_N,d->m_M);
	for (int j=0; j<d->m_N; j++)
	for (int i=0; i<d->m_M; i++) {
		ret.setValue(value(i,j),j,i);
	}
	return ret;
}
void JMatrix::setIdentity() {
	for (int j=0; j<d->m_N; j++)
	for (int i=0; i<d->m_M; i++) {
		if (i==j) setValue(1,i,j);
		else setValue(0,i,j);
	}
}
QStringList JMatrix::toStringList() const {
	QStringList ret;
	for (int i=0; i<d->m_M; i++)
	for (int j=0; j<d->m_N; j++) {
		ret << QString("%1").arg(value(i,j));
	}
	return ret;
}
void JMatrix::fromStringList(const QStringList &L) {
	int ct=0;
	for (int i=0; i<d->m_M; i++)
	for (int j=0; j<d->m_N; j++) {
		setValue(L.value(ct).toDouble(),i,j);
		ct++;
	}
}
