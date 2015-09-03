#include "foldermutex.h"
#include <QTime>
#include <QHash>
#include "qwait.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>

class FolderMutexPrivate {
public:
	FolderMutex *q;
	QString m_path;
	QFile m_file;
};


FolderMutex::FolderMutex() 
{
	d=new FolderMutexPrivate;
	d->q=this;
}

FolderMutex::~FolderMutex()
{
	delete d;
}
bool FolderMutex::lock(int timeout_ms) {
	bool ok=false;
	if (QFile(d->m_path).exists()) {
		if (QFileInfo(d->m_path).isDir()) ok=true;
		else qWarning() << "FolderMutex: Unable to lock folder: not a directory: "+d->m_path;
	}
	else qWarning() << "FolderMutex: Unable to lock folder: folder does not exist: "+d->m_path;
	if (!ok) return false;
	
	QTime timer; timer.start();
	while ((timeout_ms<=0)||(timer.elapsed()<timeout_ms)) {
		if (!QFile(d->m_file.fileName()).exists()) {
			if (d->m_file.open(QIODevice::ReadOnly)) {
				d->m_file.close();
			}
			else if (d->m_file.open(QIODevice::WriteOnly)) {
				d->m_file.write("This folder is locked.");
				//now keep the file open until we unlock
				return true;
			}
		}
		qWait(10);
	}
	return false;
}
bool FolderMutex::unlock() {
	if (d->m_file.isOpen()) {
		d->m_file.close();
		QFile::remove(d->m_file.fileName());
		return true;
	}
	return false;
}
void FolderMutex::setPath(QString path) {
	d->m_path=path;
	d->m_file.setFileName(path+"/foldermutex.lock");
}

