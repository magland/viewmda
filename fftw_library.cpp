#include "fftw_library.h"
#include <QDebug>
#include "fftw3.h"
#include <QTime>

bool internal_fftshift(Mda &X) {
	qint32 shifts[MAX_MDA_DIMS];
	for (qint32 j=0; j<X.dimCount(); j++)
		shifts[j]=X.size(j)/2;
	X.shift(shifts);
	return true;
}

bool internal_ifftshift(Mda &X) {
	qint32 shifts[MAX_MDA_DIMS];
	for (qint32 j=0; j<X.dimCount(); j++)
		shifts[j]=(X.size(j)+1)/2;
	X.shift(shifts);
	return true;
}

bool internal_fftw(Mda &X, integer dim, integer do_inverse_transform) {
	QTime time;
	time.start();
	int d1=(int)dim-1;
	qint64 N=X.size();
	if (!N) return false;

	if ((d1>=X.dimCount())||(d1<0)) {
		qWarning() << "Dimension out of range";
		return false;
	}

	fftw_complex *in;
	fftw_plan p;

	in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
	//out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);

	int n[MAX_MDA_DIMS];
	qint32 j,k;
	n[0]=X.size(d1);
	int istride=1;
	int idist=X.size(d1);
	int howmany=N/idist;

	if (do_inverse_transform) {
		p = fftw_plan_many_dft(1, n, howmany,
							in, NULL,istride, idist,
							in, NULL,istride, idist,
							FFTW_BACKWARD, FFTW_ESTIMATE);
	}
	else {
		p = fftw_plan_many_dft(1, n, howmany,
							in, NULL,istride, idist,
							in, NULL,istride, idist,
							FFTW_FORWARD, FFTW_ESTIMATE);
	}
	
	long factor1=1;
	long factor2=1;
	for (int dimind=0; dimind<d1; dimind++) {
		factor1*=X.size(dimind);
	}
	for (int dimind=d1; dimind<X.dimCount(); dimind++) {
		factor2*=X.size(dimind);
	}

	qint32 ind[MAX_MDA_DIMS];
	bool done;

	if (X.data_real) {
		real *D=X.data_real;
		long ct=0;
		for (long j1=0; j1<factor1; j1++) {
		for (long j2=0; j2<factor2; j2++) {
			in[ct][0]=D[j1+factor1*j2];
			in[ct][1]=0;
			ct++;
		}}
	}
	else if (X.data_complex) {
		complex_struct *D=X.data_complex;
		long ct=0;
		for (long j1=0; j1<factor1; j1++) {
		for (long j2=0; j2<factor2; j2++) {
			in[ct][0]=D[j1+factor1*j2].re;
			in[ct][1]=D[j1+factor1*j2].im;
			ct++;
		}}
	}
	else {
		long ct=0;
		for (long j1=0; j1<factor1; j1++) {
		for (long j2=0; j2<factor2; j2++) {
			in[ct][0]=X[j1+factor1*j2].re();
			in[ct][1]=X[j1+factor1*j2].im();
			ct++;
		}}
	}

	//printf("fftw_execute...");
	fftw_execute(p);
	//printf("fftw_execute finished.");
	

	if (X.dataType()!=MDA_TYPE_COMPLEX) {
		//printf("Converting to complex...");
		X.convertToComplex();
		//printf("Done.");
	}

	
	if (X.data_complex) {
		complex_struct *D=X.data_complex;
		long ct=0;
		for (long j2=0; j2<factor2; j2++) {
		for (long j1=0; j1<factor1; j1++) {
			D[ct].re=in[j2+factor2*j1][0];
			D[ct].im=in[j2+factor2*j1][1];
			ct++;
		}}
	}
	else {
		long ct=0;
		for (long j2=0; j2<factor2; j2++) {
		for (long j1=0; j1<factor1; j1++) {
			X[ct]=Complex(in[j2+factor2*j1][0],in[j2+factor2*j1][1]);
			ct++;
		}}
	}
	

	fftw_destroy_plan(p);
	fftw_free(in); 

	if (do_inverse_transform) {
		real factor=1.0F/X.size(d1);
		if (X.data_complex) {
			complex_struct *D=X.data_complex;
			for (j=0; j<X.size(); j++) {
				D[j].re*=factor;
				D[j].im*=factor;
			}
		}
		else {
			for (qint64 j=0; j<X.size(); j++)
				X[j]=X[j]*factor;
		}
	}
	
	return true;
}

bool fftshift(Mda &ret, Mda &X) {
	ret=X;
	internal_fftshift(ret);
	return true;
}

bool ifftshift(Mda &ret, Mda &X) {
	ret=X;
	internal_ifftshift(ret);
	return true;
}	
