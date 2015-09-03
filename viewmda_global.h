#ifndef viewmda_global_H
#define viewmda_global_H

#ifdef VIEWMDA
	#define VIEWMDA_EXPORT Q_DECL_EXPORT
#else
	#define VIEWMDA_EXPORT Q_DECL_IMPORT
#endif

#endif
