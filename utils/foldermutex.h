#ifndef foldermutex_H
#define foldermutex_H

#include <QString>

class FolderMutexPrivate;
class FolderMutex {
public:
	friend class FolderMutexPrivate;
	FolderMutex();
	virtual ~FolderMutex();
	void setPath(QString path);
	bool lock(int timeout_ms=1000);
	bool unlock();
private:
	FolderMutexPrivate *d;
};

#endif
