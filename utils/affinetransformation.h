#ifndef affinetransformation_H
#define affinetransformation_H


#include <QList>
#include "jmatrix.h"
#include "array3d.h"
#include <math.h>

#ifndef PI
#define PI 3.141592
#endif


class AffineTransformationPrivate;
class AffineTransformation {
public:
	friend class AffineTransformationPrivate;
	AffineTransformation();
	AffineTransformation(const AffineTransformation &X);
	virtual ~AffineTransformation();
	Pt3f map(Pt3f pt) const;
	AffineTransformation inverted() const;
	void translate(Pt3f pt);
	void translate(float dx,float dy,float dz);
	void scale(Pt3f factor);
	void scale(float sx,float sy,float sz);
	void scaleLeft(float sx,float sy,float sz);
	void scaleRight(float sx,float sy,float sz);
	void xRotate(float deg);
	void yRotate(float deg);
	void zRotate(float deg);
	void setIdentity();
	void leftComposeWith(const AffineTransformation &T);
	QList<float> matrix() const;
	float matrixValue(int i,int j) const;
	void setMatrixValue(float val,int i,int j);
	void setMatrix(const QList<float> &X);
	void setMatrix(const JMatrix &M);
	void operator=(const AffineTransformation &X);
	void display() const;
	QString displayText() const;
	bool isIdentity() const;
	bool isNull() const;
	bool operator==(const AffineTransformation &X);
	AffineTransformation operator*(const AffineTransformation &X) const;
	QStringList toStringList(bool row_major=true) const;
	void fromStringList(const QStringList &L,bool row_major=true);
	QString toText() const;
	void writeMda(QString mda_fname) const;
	void readMda(QString mda_fname);
	void loadFromTextFile(QString fname);
	void saveToTextFile(QString fname);
private:
	AffineTransformationPrivate *d;
};

#endif
