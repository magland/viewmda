#ifndef jarchive_H
#define jarchive_H

#define JARCHIVE_MODE_READ 1
#define JARCHIVE_MODE_WRITE 2
#define JARCHIVE_MODE_APPEND 3

#include <QString>
#include <QByteArray>
#include <QVariant>
#include <QStringList>

class JArchivePrivate;
class JArchive {
public:
	friend class JArchivePrivate;
	JArchive(const QString &fname);
	JArchive(QIODevice *DD,int open_mode);
	virtual ~JArchive();
	bool open(int open_mode);
	void close();
	bool writeData(const QString &key,const QByteArray &data,bool compress=true,const QString &data_type="");
	QByteArray readData(const QString &key);
	bool writeValue(const QString &key,const QVariant &value,bool compress=false);
	QVariant readValue(const QString &key,const QVariant &defaultval=QVariant());
	long dataSize(const QString &key); //in bytes
	void setValue(const QString &key,const QVariant &value,bool compress=false); //same as writeValue
	QVariant value(const QString &key,const QVariant &defaultval=QVariant()); //same as readValue
	QStringList allKeys() const;
	QStringList childKeys() const;
	QString dataType(const QString &key) const;
	void beginGroup(const QString &prefix,const QString &custom_type="");
	void endGroup();
	int beginReadArray(const QString &prefix);
	void beginWriteArray(const QString &prefix,int size);
	void setArrayIndex(int i);
	void endArray();
	bool addFile(const QString &key,const QString &filename,bool compress=true);
	bool extractFile(const QString &key,const QString &filename);	
	bool contains(const QString &key);
	QString fileName() const;
	
	void writeFloatArray(const QString &key,const QList<float> &X,bool compress=true);
	void writeFloatArray(const QString &key,const QVector<float> &X,bool compress=true);
	QList<float> readFloatArray(const QString &key);
	void writeFloatArray(const QString &key,float *X,qint32 N,bool compress=true);
	void readFloatArray(const QString &key,float *X,qint32 N);
	void writeDoubleArray(const QString &key,const QList<double> &X,bool compress=true);
	void writeDoubleArray(const QString &key,double *X,qint32 N,bool compress=true);
	QList<double> readDoubleArray(const QString &key);
	void readDoubleArray(const QString &key,double *X,qint32 N);
	QStringList readStringList(const QString &key);
	void writeStringList(const QString &key,const QStringList &X,bool compress=true);
	void writeInt16Array(const QString &key,qint16 *X,qint32 N,bool compress=true);
	void readInt16Array(const QString &key,qint16 *X,qint32 N);
	void writeUint16Array(const QString &key,quint16 *X,qint32 N,bool compress=true);
	void readUint16Array(const QString &key,quint16 *X,qint32 N);
private:
	JArchivePrivate *d;
};



#endif
