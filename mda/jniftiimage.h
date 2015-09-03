#ifndef jniftiimage_H
#define jniftiimage_H

#include <QList>
#include "array3d.h"
#include "array4d.h"

extern "C" {
	#include "niftilib/nifti1.h"
}

class JNiftiImagePrivate;
class JNiftiImage {
public:
	friend class JNiftiImagePrivate;
	JNiftiImage();
	JNiftiImage(const JNiftiImage &X);
	virtual ~JNiftiImage();
	void clear();
	bool read(QString nii_fname,bool load_data=true);
	bool write(QString nii_fname);
	void loadData(); 
	int dimCount() const;
	QList<int> size() const;
	int size(int dim) const;
	QList<float> resolution() const;
	float resolution(int dim) const;
	void setResolution(float dx,float dy=1,float dz=1,float dt=1,float du=1,float dv=1,float dw=1);
	void setResolution(const QList<float> &list);
	Array2D toArray2D() const;
	Array3D toArray3D() const;
	Array4D toArray4D() const;
	Mda toMda() const;
	float value(int x,int y=0,int z=0,int t=0,int u=0,int v=0,int w=0) const;
	void setValue(float val,int x,int y=0,int z=0,int t=0,int u=0,int v=0,int w=0);
	Pt3f xyzPosition(int indx,int indy,int indz) const;
	Pt3f ijkPosition(float x,float y,float z) const;
	QString description() const;

	void allocate(int nifti_datatype,int dim1,int dim2=-1,int dim3=-1,int dim4=-1,int dim5=-1,int dim6=-1,int dim7=-1);
	void allocate(int nifti_datatype,const QList<int> &dims);
	int datatype() const;
	void fromArray2D(const Array2D &X,int datatype);
	void fromArray3D(const Array3D &X,int datatype);
	void fromArray4D(const Array4D &X,int datatype);
	void writeMda(QString mda_fname) const;
	
	void operator=(const JNiftiImage &X);
	
	QString debugString() const;
private:
	JNiftiImagePrivate *d;
};

#endif
