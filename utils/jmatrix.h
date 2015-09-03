#ifndef jmatrix_H
#define jmatrix_H
#include <QString>
#include <QStringList>

class JMatrixPrivate;
class JMatrix {
public:
	friend class JMatrixPrivate;
	JMatrix(int M=0,int N=0);
	JMatrix(const JMatrix &M);
	virtual ~JMatrix();
	void allocate(int M,int N);
	int M() const;
	int N() const;
	double value(int i,int j) const;
	void setValue(double val,int i,int j);
	void operator=(const JMatrix &M);
	JMatrix operator*(const JMatrix &M);
	JMatrix inverse() const;
	JMatrix transpose() const;
	void setIdentity();
	QStringList toStringList() const;
	void fromStringList(const QStringList &L);
	#ifdef USING_ARRAY3D
	void write(QString fname);
	void read(QString fname);
	#endif
private:
	JMatrixPrivate *d;
};




#endif
