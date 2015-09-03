#ifndef jniftiimage2_H
#define jniftiimage2_H

#include "affinetransformation.h"
#include <QMap>
#include <QVariant>

class JNiftiImage2Private;
class JNiftiImage2 {
public:
	friend class JNiftiImage2Private;
	JNiftiImage2();
	JNiftiImage2(const JNiftiImage2 &X);
	virtual ~JNiftiImage2();
	void operator=(const JNiftiImage2 &X);
	
	bool read(QString nii_fname,bool load_data=true);
	bool write(QString nii_fname);
	void clear();
	
	int N1() const;
	int N2() const;
	int N3() const;
	int N4() const;
	bool allocate(QString data_type,int N1,int N2,int N3,int N4=1);
	//data_type = "uchar", "int16", "int32", "int64", "real32"
	Array3D dataXYZ(int t) const;
	Array3D dataXYT(int z) const;
	void setDataXYZ(const Array3D &X,int t);
	void setDataXYT(const Array3D &X,int z);
	QString dataType() const;
	
	QMap<QString,QVariant> headerParameters() const;
	
	
	AffineTransformation worldTransformation() const;
	void setWorldTransformation(const AffineTransformation &T);
private:
	JNiftiImage2Private *d;
};

#endif
