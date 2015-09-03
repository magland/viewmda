#include "jniftiimage.h"
#include <QDebug>
#include <QFileInfo>

extern "C" {
	#include "niftilib/nifti1_io.h"
}
#include "mda.h"


class JNiftiImagePrivate {
public:
	JNiftiImage *q;
	nifti_image *m_image;	
	bool m_data_loaded;
	
	long get_index(int x,int y,int z,int t,int u,int v,int w);
	void copy_from(const JNiftiImage &X);
};

JNiftiImage::JNiftiImage() 
{
	d=new JNiftiImagePrivate;
	d->q=this;
	d->m_image=0;
	d->m_data_loaded=false;
}
JNiftiImage::JNiftiImage(const JNiftiImage &X) {
	d=new JNiftiImagePrivate;
	d->q=this;
	d->m_image=0;
	d->m_data_loaded=false;
	d->copy_from(X);
}

JNiftiImage::~JNiftiImage()
{
	clear();
	delete d;
}
void JNiftiImagePrivate::copy_from(const JNiftiImage &X) {
	q->allocate(X.datatype(),X.size());
	memcpy(m_image->data,X.d->m_image->data,m_image->nbyper*m_image->nvox);
	q->setResolution(X.resolution());
}
bool JNiftiImage::read(QString fname,bool load_data) {
	clear();
	QString fname2=fname;
	#ifdef __WIN32__
	fname2.replace("/","\\");
	#endif
    printf("Reading %s...",fname.toLatin1().data());
    d->m_image=nifti_image_read(fname2.toLatin1().data(),0);
	if (!d->m_image) {
		printf("Error reading niftii image.\n");
		return false;
	}
	printf("Loading data...");
	if (load_data) loadData();
	printf("[].\n");
	return true;
}
void JNiftiImage::loadData() {
	if (!d->m_image) return;
	if (d->m_data_loaded) return;
	nifti_image_load(d->m_image);
	d->m_data_loaded=true;
}
void JNiftiImage::clear() {
	if (d->m_image) {
		nifti_image_unload(d->m_image);
		nifti_image_free(d->m_image);
		d->m_image=0;
		d->m_data_loaded=false;
	}	
}
QList<int> JNiftiImage::size() const {
	QList<int> ret;
	for (int i=0; i<dimCount(); i++)
		ret << size(i);
	return ret;
}
int JNiftiImage::size(int dim) const {
	if (!d->m_image) return 0;
	if (dim>=dimCount()) return 1;
	return d->m_image->dim[dim+1];
}
int JNiftiImage::dimCount() const {
	if (!d->m_image) return 0;
	return d->m_image->ndim;
}
#define MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_1  \
	long ct=0; \
	for (int y=0; y<size(1); y++) \
	for (int x=0; x<size(0); x++) { \
		ret.setValue(ddd[ct],x,y); \
		ct++; \
	}
	
Array2D JNiftiImage::toArray2D() const {
	Array2D ret(size(0),size(1));
	int nbyper,swapsize;
	nifti_datatype_sizes(d->m_image->datatype,&nbyper,&swapsize);
	if (nifti_is_inttype(d->m_image->datatype)) {
		if (nbyper==1) {
			unsigned char *ddd=(unsigned char *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_1
		}
		else if (nbyper==2) {
			quint16 *ddd=(quint16 *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_1
		}
		else if (nbyper==4) {
			qint32 *ddd=(qint32 *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_1
		}
		else if (nbyper==8) {
			qint64 *ddd=(qint64 *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_1
		}
	}
	else {
		if (d->m_image->datatype==DT_FLOAT32) {
			float *ddd=(float *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_1
		}
		//floating point not supported yet
	}
	
	return ret;
}
#define MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_2 \
	long ct=0; \
	for (int z=0; z<size(2); z++) \
	for (int y=0; y<size(1); y++) \
	for (int x=0; x<size(0); x++) { \
		ret.setValue(ddd[ct],x,y,z); \
		ct++; \
	}
	
Array3D JNiftiImage::toArray3D() const {
	Array3D ret(size(0),size(1),size(2));
	int nbyper,swapsize;
	nifti_datatype_sizes(d->m_image->datatype,&nbyper,&swapsize);
	if (nifti_is_inttype(d->m_image->datatype)) {
		if (nbyper==1) {
			unsigned char *ddd=(unsigned char *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_2
		}
		else if (nbyper==2) {
			quint16 *ddd=(quint16 *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_2
		}
		else if (nbyper==4) {
			qint32 *ddd=(qint32 *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_2
		}
		else if (nbyper==8) {
			qint64 *ddd=(qint64 *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_2
		}
	}
	else {
		if (d->m_image->datatype==DT_FLOAT32) {
			float *ddd=(float *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_2
		}
	}
	
	return ret;
}
#define MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_3 \
	long ct=0; \
	for (int t=0; t<size(3); t++) \
	for (int z=0; z<size(2); z++) \
	for (int y=0; y<size(1); y++) \
	for (int x=0; x<size(0); x++) { \
		ret.setValue(ddd[ct],x,y,z,t); \
		ct++; \
	}
Array4D JNiftiImage::toArray4D() const {
	Array4D ret(size(0),size(1),size(2),size(3));
	int nbyper,swapsize;
	nifti_datatype_sizes(d->m_image->datatype,&nbyper,&swapsize);
	if (nifti_is_inttype(d->m_image->datatype)) {
		if (nbyper==1) {
			unsigned char *ddd=(unsigned char *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_3
		}
		else if (nbyper==2) {
			quint16 *ddd=(quint16 *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_3
		}
		else if (nbyper==4) {
			qint32 *ddd=(qint32 *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_3
		}
		else if (nbyper==8) {
			qint64 *ddd=(qint64 *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_3
		}
	}
	else {
		if (d->m_image->datatype==DT_FLOAT32) {
			float *ddd=(float *)(d->m_image->data);
			MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_3
		}
	}
	
	return ret;
}
Mda JNiftiImage::toMda() const {
	Array4D tmp=toArray4D();
	Mda ret;
	ret.allocate(MDA_TYPE_REAL,tmp.width(),tmp.height(),tmp.numSlices(),tmp.numFrames());
	for (long i=0; i<ret.size(); i++) {
		ret(i)=tmp.value1(i);
	}
	return ret;
}
QList<float> JNiftiImage::resolution() const {
	QList<float> ret;
	for (int i=0; i<dimCount(); i++)
		ret << resolution(i);
	return ret;	
}
float JNiftiImage::resolution(int dim) const {
	if (!d->m_image) return 0;
	return d->m_image->pixdim[dim+1];
}
QString mat44ToString(const mat44 &M) {
	QString ret;
	for (int i=0; i<4; i++) {
		ret+="[ ";
		for (int j=0; j<4; j++) {	
			ret+=QString("%1 ").arg(M.m[i][j]);
		}
		ret+=" ];  ";
	}
	return ret;
}
QString JNiftiImage::debugString() const {
	if (!d->m_image) return "";
	nifti_image *II=d->m_image;
	QString ret;
	ret+=QString("ndim=%1, nx,ny,nz,nt,nu,nv,nw=(%2,%3,%4,%5,%6,%7,%8)\n")
			.arg(II->ndim).arg(II->nx).arg(II->ny).arg(II->nz).arg(II->nt).arg(II->nu).arg(II->nv).arg(II->nw);
	ret+=QString("nbyper=%1, datatype=%2\n").arg(II->nbyper).arg(nifti_datatype_string(II->datatype));
	ret+=QString("dx,dy,dz,dt,du,dv,dw=(%1,%2,%3,%4,%5,%6,%7)\n")
			.arg(II->dx).arg(II->dy).arg(II->dz).arg(II->dt).arg(II->du).arg(II->dv).arg(II->dw);
	ret+=QString("scl_slope=%1, scl_inter=%2\n").arg(II->scl_slope).arg(II->scl_inter);
	ret+=QString("cal_min=%1, cal_max=%2\n").arg(II->cal_min).arg(II->cal_max);
	ret+=QString("qform_code=%1, sform_code=%2\n").arg(II->qform_code).arg(II->sform_code);
	ret+=QString("freq_dim=%1, phase_dime=%2, slice_dim=%3\n").arg(II->freq_dim).arg(II->phase_dim).arg(II->slice_dim);
	
	ret+=QString("slice_code=%1, slice_start=%2, slice_end=%3, slice_duration=%4\n")
			.arg(II->slice_code).arg(II->slice_start).arg(II->slice_end).arg(II->slice_duration);
	ret+=QString("quatern_(b,c,d)=(%1,%2,%3), qoffset_(x,y,z)=(%4,%5,%6), qfac=%7\n")
			.arg(II->quatern_b).arg(II->quatern_c).arg(II->quatern_d)
			.arg(II->qoffset_x).arg(II->qoffset_y).arg(II->qoffset_z)
			.arg(II->qfac);
	ret+=QString("qto_xyz=%1\n").arg(mat44ToString(II->qto_xyz));
	ret+=QString("qto_ijk=%1\n").arg(mat44ToString(II->qto_ijk));
	ret+=QString("sto_xyz=%1\n").arg(mat44ToString(II->sto_xyz));
	ret+=QString("sto_ijk=%1\n").arg(mat44ToString(II->sto_ijk));
	ret+=QString("toffset=%1\n").arg(II->toffset);
	ret+=QString("xyz_units=%1, time_units=%2\n").arg(nifti_units_string(II->xyz_units)).arg(nifti_units_string(II->time_units));
  
	ret+=QString("nifti_type=%1, 0=analyze, 1=nifti1 single, 2=nifti-1 two files, 3=nifti-ascii\n").arg(II->nifti_type);

	ret+=QString("intent_code=%1\n").arg(nifti_intent_string(II->intent_code));
	ret+=QString("intent_params=(%1,%2,%3)\n").arg(II->intent_p1).arg(II->intent_p2).arg(II->intent_p3);
	ret+=QString("intent_name=%1\n").arg(II->intent_name);
	ret+=QString("descrip=%1\n").arg(II->descrip);
	ret+=QString("aux_file=%1\n").arg(II->aux_file);
	ret+=QString("fname=%1\n").arg(II->fname);
	ret+=QString("iname=%1\n").arg(II->iname);
	ret+=QString("iname_offset=%1, swapsize=%2, byteorder=%3\n").arg(II->iname_offset).arg(II->swapsize).arg(II->byteorder);
	ret+=QString("num_ext=%1\n").arg(II->num_ext);
	
	ret+=QString("analyze75_orient=%1\n").arg(II->analyze75_orient);

	return ret;
}
bool JNiftiImage::write(QString nii_fname) {
	if (!d->m_image) return false;
	QString prefix=QFileInfo(nii_fname).path()+"/"+QFileInfo(nii_fname).completeBaseName();
	#ifdef __WIN32__
	prefix.replace("/","\\");
	#endif
    nifti_set_filenames(d->m_image,prefix.toLatin1().data(),0,1);
	QFile::remove(d->m_image->fname);
	nifti_set_iname_offset(d->m_image);
	nifti_image_write(d->m_image);
	return true;
}
void set_identity(mat44 &M) {
	for (int i=0; i<4; i++)
	for (int j=0; j<4; j++) {
		if (i==j) M.m[i][j]=1;
		else M.m[i][j]=0;
	}
}
#define MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_4 \
	for (unsigned long ct=0; ct<d->m_image->nvox; ct++) { \
		ddd[ct]=X.value1(ct); \
	}
	
void JNiftiImage::fromArray2D(const Array2D &X,int datatype) {
	allocate(datatype,X.width(),X.height());
	if (datatype==DT_INT16) {	
		quint16 *ddd=(quint16 *)d->m_image->data;
		MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_4
	}
	else if (datatype==DT_UINT8) {	
		unsigned char *ddd=(unsigned char *)d->m_image->data;
		MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_4
	}
	else if (datatype==DT_FLOAT32) {
		float *ddd=(float *)d->m_image->data;
		MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_4
	}
}
void JNiftiImage::fromArray3D(const Array3D &X,int datatype) {
	allocate(datatype,X.width(),X.height(),X.numSlices());
	if (datatype==DT_INT16) {	
		quint16 *ddd=(quint16 *)d->m_image->data;
		MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_4
	}
	else if (datatype==DT_UINT8) {	
		unsigned char *ddd=(unsigned char *)d->m_image->data;
		MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_4
	}
	else if (datatype==DT_FLOAT32) {
		float *ddd=(float *)d->m_image->data;
		MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_4
	}
}
void JNiftiImage::fromArray4D(const Array4D &X,int datatype) {
	allocate(datatype,X.width(),X.height(),X.numSlices(),X.numFrames());
	if (datatype==DT_INT16) {	
		quint16 *ddd=(quint16 *)d->m_image->data;
		MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_4
	}
	else if (datatype==DT_UINT8) {	
		unsigned char *ddd=(unsigned char *)d->m_image->data;
		MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_4
	}
	else if (datatype==DT_FLOAT32) {
		float *ddd=(float *)d->m_image->data;
		MACRO_HACK_ONLY_USED_ONCE_IN_MY_LIFE_4
	}
}
void JNiftiImage::allocate(int datatype,int dim1,int dim2,int dim3,int dim4,int dim5,int dim6,int dim7) {
	QList<int> dims; dims << dim1 << dim2 << dim3 << dim4 << dim5 << dim6 << dim7;
	for (int i=0; i<dims.count(); i++) if (dims[i]<=0) {dims.removeAt(i); i--;}	
	allocate(datatype,dims);
}
void JNiftiImage::allocate(int datatype,const QList<int> &dims) {
	clear();
	d->m_image = (nifti_image *)calloc( 1 , sizeof(nifti_image) ) ;
	if (!d->m_image) return;
	nifti_image *NI=d->m_image;
	
	NI->ndim=dims.count();
	NI->dim[0]=NI->ndim;
	for (int i=0; i<7; i++) {
		if (i<dims.count()) NI->dim[i+1]=dims[i];
		else NI->dim[i+1]=1;
	}
	NI->nx=NI->dim[1]; NI->ny=NI->dim[2]; NI->nz=NI->dim[3]; 
	NI->nt=NI->dim[4]; NI->nu=NI->dim[5]; NI->nv=NI->dim[6]; NI->nw=NI->dim[7];

	NI->nvox=1;
	for (int i=1; i<=NI->ndim; i++) NI->nvox*=NI->dim[i];
	NI->nbyper=2;
	NI->datatype=datatype;
	if (datatype==DT_FLOAT32) NI->nbyper=4;
	else if (datatype==DT_UINT8) NI->nbyper=1;

  	NI->dx=NI->dy=NI->dz=NI->dt=NI->du=NI->dv=NI->dw=1;
	for (int i=0; i<8; i++) NI->pixdim[i]=1;

  	NI->scl_slope=0; NI->scl_inter=0;
	
	NI->cal_min=NI->cal_max=0;
	
	NI->qform_code=0; NI->sform_code=1;
	
	NI->freq_dim=0; NI->phase_dim=0; NI->slice_dim=0;
	
	NI->slice_code=0; NI->slice_start=0; NI->slice_end=0; NI->slice_duration=0;
	
	NI->quatern_b=NI->quatern_c=NI->quatern_d=0;
	NI->qoffset_x=NI->qoffset_y=NI->qoffset_z=0;
	NI->qfac=0;
	
	set_identity(NI->qto_xyz); set_identity(NI->qto_ijk);
	set_identity(NI->sto_xyz); set_identity(NI->sto_ijk);
	for (int i=0; i<3; i++) {
		NI->sto_xyz.m[i][3]=1;
		NI->sto_ijk.m[i][3]=-1;
	}

  	NI->toffset=0;
  	
  	NI->xyz_units=0; NI->time_units=0;

	NI->nifti_type=1;
	
	NI->intent_code=0;
	NI->intent_p1=NI->intent_p2=NI->intent_p3=0;
	
	strcpy(NI->intent_name,"");
  	strcpy(NI->descrip,"");
  	strcpy(NI->aux_file,"none");

  	NI->fname=0; //is this right?
  	NI->iname=0;
	
  	NI->iname_offset=352; //this will be set later, in write()
  	NI->swapsize=2; //hard code this for now
  	NI->byteorder=1; //this should be set later, in write()
  	long total_bytes=NI->nvox*NI->nbyper;
  	NI->data=(void *)calloc(1,total_bytes) ;  /* create image memory */
  	{
  		if (datatype==DT_INT16) {
  			quint16 *ddd=(quint16 *)NI->data;
	  		for (unsigned long ct=0; ct<NI->nvox; ct++) {
	  			ddd[ct]=0;
	  		}
  		}
  		else if (datatype==DT_UINT8) {
  			unsigned char *ddd=(unsigned char *)NI->data;
	  		for (unsigned long ct=0; ct<NI->nvox; ct++) {
	  			ddd[ct]=0;
	  		}
  		}
  		else if (datatype==DT_FLOAT32) {
  			float *ddd=(float *)NI->data;
	  		for (unsigned long ct=0; ct<NI->nvox; ct++) {
	  			ddd[ct]=0;
	  		}
  		}
  	}
  	NI->num_ext=0;
  	NI->ext_list=0;

  	NI->analyze75_orient=a75_transverse_unflipped;
}

long JNiftiImagePrivate::get_index(int x,int y,int z,int t,int u,int v,int w) {
	int ind[7]; 
	ind[0]=x; ind[1]=y; ind[2]=z;
	if (q->dimCount()>3) {
		ind[3]=t;
		if (q->dimCount()>4) {
			ind[4]=u; ind[5]=v; ind[6]=w;
		}
	}
	long ret=0;
	long factor=1;
	for (int i=0; i<q->dimCount(); i++) {
		ret+=ind[i]*factor;
		factor*=q->size(i);
	}
	return ret;
}
float JNiftiImage::value(int x,int y,int z,int t,int u,int v,int w) const {
	if (!d->m_image) return 0;
	long ind=d->get_index(x,y,z,t,u,v,w);
	if (d->m_image->datatype==DT_INT16) {
		quint16 *ddd=(quint16 *)d->m_image->data;
		return ddd[ind];
	}
	else if (d->m_image->datatype==DT_UINT8) {
		unsigned char *ddd=(unsigned char *)d->m_image->data;
		return ddd[ind];
	}
	else if (d->m_image->datatype==DT_FLOAT32) {
		float *ddd=(float *)d->m_image->data;
		return ddd[ind];
	}
	else return 0;
}
void JNiftiImage::setValue(float val,int x,int y,int z,int t,int u,int v,int w) {
	if (!d->m_image) return;
	long ind=d->get_index(x,y,z,t,u,v,w);
	if (d->m_image->datatype==DT_INT16) {
		quint16 *ddd=(quint16 *)d->m_image->data;
		ddd[ind]=(quint16)val;
	}
	else if (d->m_image->datatype==DT_UINT8) {
		unsigned char *ddd=(unsigned char *)d->m_image->data;
		ddd[ind]=(quint16)val;
	}
	else if (d->m_image->datatype==DT_FLOAT32) {
		float *ddd=(float *)d->m_image->data;
		ddd[ind]=val;
	}
}
void JNiftiImage::setResolution(float dx,float dy,float dz,float dt,float du,float dv,float dw) {
	if (!d->m_image) return;
	d->m_image->dx=d->m_image->pixdim[1]=qAbs(dx);
	d->m_image->dy=d->m_image->pixdim[2]=qAbs(dy);
	d->m_image->dz=d->m_image->pixdim[3]=qAbs(dz);
	d->m_image->dt=d->m_image->pixdim[4]=qAbs(dt);
	d->m_image->du=d->m_image->pixdim[5]=qAbs(du);
	d->m_image->dv=d->m_image->pixdim[6]=qAbs(dv);
	d->m_image->dw=d->m_image->pixdim[7]=qAbs(dw);
	d->m_image->sto_xyz.m[0][0]=dx;
	d->m_image->sto_xyz.m[1][1]=dy;
	d->m_image->sto_xyz.m[2][2]=dz;
}
void JNiftiImage::setResolution(const QList<float> &list) {
	setResolution(list.value(0,1),list.value(1,1),list.value(2,1),list.value(3,1),list.value(4,1),list.value(5,1),list.value(6,1));
}
void JNiftiImage::operator=(const JNiftiImage &X) {
	d->copy_from(X);
}
void JNiftiImage::writeMda(QString mda_fname) const {
	if (!d->m_image) return;
	QString fname=mda_fname;
	#ifdef WIN32
	fname.replace("/","\\");
	#endif
    FILE *outf=fopen(fname.toLatin1().data(),"wb");
	if (!outf) return;
	qint32 mda_data_type=0;
	qint32 num_bytes=0;
	{		
		if (d->m_image->datatype==DT_INT16) {
			mda_data_type=MDA_TYPE_SHORT;
			num_bytes=2;
		}
		else if (d->m_image->datatype==DT_UINT8) {
			mda_data_type=MDA_TYPE_BYTE;
			num_bytes=1;
		}
		else if (d->m_image->datatype==DT_FLOAT32) {
			mda_data_type=MDA_TYPE_REAL;
			num_bytes=4;
		}
		else {
		}
		fwrite(&mda_data_type,sizeof(qint32),1,outf);
		fwrite(&num_bytes,sizeof(qint32),1,outf);
	}
	qint32 num_dims=dimCount();
	fwrite(&num_dims,sizeof(qint32),1,outf);
	qint32 j;
	for (j=0; j<num_dims; j++) {
		qint32 holdval=size(j);
		fwrite(&holdval,sizeof(qint32),1,outf);
	}
	qint32 N=d->m_image->nvox;
	if (d->m_image->datatype==DT_INT16) {
		quint16 *ddd=(quint16 *)d->m_image->data;
		for (j=0; j<N; j++) {
			fwrite(&ddd[j],num_bytes,1,outf);
		}
	}
	else if (d->m_image->datatype==DT_UINT8) {
		unsigned char *ddd=(unsigned char *)d->m_image->data;
		for (j=0; j<N; j++) {
			fwrite(&ddd[j],num_bytes,1,outf);
		}
	}
	else if (d->m_image->datatype==DT_FLOAT32) {
		float *ddd=(float *)d->m_image->data;
		for (j=0; j<N; j++) {
			fwrite(&ddd[j],num_bytes,1,outf);
		}
	}
	qint32 com=0;
	fwrite(&com,sizeof(qint32),1,outf);
	fclose(outf);
}
int JNiftiImage::datatype() const {
	if (!d->m_image) return 0;
	return d->m_image->datatype;
}
Pt3f JNiftiImage::xyzPosition(int indx,int indy,int indz) const {
	if (!d->m_image) return pt3f(0,0,0);
	nifti_image *II=d->m_image;
	Pt3f ret=pt3f(0,0,0);
	ret.x=II->sto_xyz.m[0][0]*indx+II->sto_xyz.m[0][1]*indy+II->sto_xyz.m[0][2]*indz+II->sto_xyz.m[0][3];
	ret.y=II->sto_xyz.m[1][0]*indx+II->sto_xyz.m[1][1]*indy+II->sto_xyz.m[1][2]*indz+II->sto_xyz.m[1][3];
	ret.z=II->sto_xyz.m[2][0]*indx+II->sto_xyz.m[2][1]*indy+II->sto_xyz.m[2][2]*indz+II->sto_xyz.m[2][3];
	return ret;
}
Pt3f JNiftiImage::ijkPosition(float x,float y,float z) const {
	if (!d->m_image) return pt3f(0,0,0);
	nifti_image *II=d->m_image;
	Pt3f ret=pt3f(0,0,0);
	ret.x=II->sto_ijk.m[0][0]*x+II->sto_ijk.m[0][1]*y+II->sto_ijk.m[0][2]*z+II->sto_ijk.m[0][3];
	ret.y=II->sto_ijk.m[1][0]*x+II->sto_ijk.m[1][1]*y+II->sto_ijk.m[1][2]*z+II->sto_ijk.m[1][3];
	ret.z=II->sto_ijk.m[2][0]*x+II->sto_ijk.m[2][1]*y+II->sto_ijk.m[2][2]*z+II->sto_ijk.m[2][3];
	return ret;
}
QString JNiftiImage::description() const {
	nifti_image *II=d->m_image;
	return II->descrip;
}






