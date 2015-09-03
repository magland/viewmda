#include "jarchive.h"
#include <QFile>
#include <QHash>
#include <QDebug>
#include <QDataStream>
#include <QFileInfo>
#include <QBuffer>

#include <QImage>
#include <QDateTime>
#include <QTime>

#define JARCHIVE_VERSION 3

#define JARCHIVE_MODE_CLOSED 0

#define JARCHIVE_BEGIN_FILE_CODE 21
#define JARCHIVE_BEGIN_RECORD_CODE 22


struct JArchiveRecordInfo {
	QString key;
	QString data_type;
	qint64 pos;
	qint32 length_of_data;
	qint32 compressed_length_of_data;
	bool is_compressed;
};

struct JArchivePrefix {
	QString name;
	int type; //1=group, 2=array, 3=array index
};

class JArchivePrivate {
public:
	JArchive *q;
	QIODevice *m_file;
	QString m_file_name;
	int m_open_mode;
	QHash<QString,JArchiveRecordInfo> m_records;
	QList<QString> m_record_keys; //in the order in which they appear
	QString m_group_prefix;
	QList<JArchivePrefix> m_current_prefix_path;
	float m_read_version_number;
	
	bool write_header();
	bool read_contents();
	
	void write_bytes(const QByteArray &X);
	void write_int32(qint32 x);
	QByteArray read_bytes(qint32 num);
	qint32 read_int32();
};

JArchive::JArchive(const QString &fname) 
{
	d=new JArchivePrivate;
	d->q=this;
	d->m_file=0;
	d->m_open_mode=JARCHIVE_MODE_CLOSED;
	d->m_file_name=fname;
	d->m_read_version_number=JARCHIVE_VERSION;
}
JArchive::JArchive(QIODevice *DD,int open_mode) {
	d=new JArchivePrivate;
	d->q=this;
	d->m_file=DD;
	d->m_open_mode=open_mode;
	d->m_file_name="";
	d->m_read_version_number=JARCHIVE_VERSION;
	if (d->m_open_mode==JARCHIVE_MODE_READ) {
		d->read_contents();
	}
	else if (d->m_open_mode==JARCHIVE_MODE_WRITE) {
		d->write_header();
	}
}

JArchive::~JArchive()
{
	delete d;
}

bool JArchive::open(int open_mode) {
	if (d->m_file) delete d->m_file;
	d->m_file=new QFile(d->m_file_name);
	if (open_mode==JARCHIVE_MODE_WRITE) {
		if (!d->m_file->open(QIODevice::WriteOnly)) return false;
		d->write_header();
	}
	else if (open_mode==JARCHIVE_MODE_APPEND) {
		if (!d->m_file->open(QIODevice::WriteOnly|QIODevice::Append)) return false;
	}
	else if (open_mode==JARCHIVE_MODE_READ) {
		if (!d->m_file->open(QIODevice::ReadOnly)) return false;
		d->read_contents();
	}
	else return false;
	d->m_open_mode=open_mode;
	return true;
}
void JArchive::close() {	
	if (d->m_file) {
		d->m_file->close();
		d->m_file->deleteLater();
		d->m_file=0;
	}
	d->m_open_mode=JARCHIVE_MODE_CLOSED;
}
bool JArchivePrivate::write_header() {
	qint32 version_number=JARCHIVE_VERSION;
	qint32 header_size=4*3;
	qint32 code=JARCHIVE_BEGIN_FILE_CODE;
	
	write_int32(code); //4
	write_int32(header_size); //4
	write_int32(version_number); //4
	return true;
}
bool JArchivePrivate::read_contents() {
	if (!m_file) return false;
	m_records.clear();
	m_record_keys.clear();
	m_file->seek(0);
	qint32 header_size,version_number;
	{
		qint32 code=read_int32(); 
		if (code!=JARCHIVE_BEGIN_FILE_CODE) {
			qWarning() << "Error in JArchivePrivate::read_contents. JARCHIVE_BEGIN_FILE_CODE does not match" << code << JARCHIVE_BEGIN_FILE_CODE;
			return false;
		}
		header_size=read_int32();
		version_number=read_int32();
	}
	m_read_version_number=version_number;
	
	m_file->seek(header_size);
	qint64 filepos=m_file->pos();
	bool done=false;
	while ((!m_file->atEnd())&&(!done)) {
		qint32 code=read_int32();
		if (code==JARCHIVE_BEGIN_RECORD_CODE) {
			qint32 length_of_header,length_of_key,is_compressed;
			qint32 length_of_data,compressed_length_of_data;
			QByteArray key;
			qint32 length_of_data_type;
			QByteArray data_type;
			if (version_number<=1) {
				length_of_header=read_int32(); //4
				compressed_length_of_data=read_int32(); //4
				length_of_data=read_int32(); //4
				length_of_key=read_int32(); //4
				key=read_bytes(length_of_key);
				is_compressed=read_int32(); //4
			}
			else {
				length_of_header=read_int32(); //4
				compressed_length_of_data=read_int32(); //4
				length_of_data=read_int32(); //4
				length_of_key=read_int32(); //4
				key=read_bytes(length_of_key); //length_of_key
				is_compressed=read_int32(); //4
				length_of_data_type=read_int32(); //4
				data_type=read_bytes(length_of_data_type); //4
			}
			JArchiveRecordInfo R;
			R.compressed_length_of_data=compressed_length_of_data;
			R.is_compressed=(is_compressed!=0);
			R.key=QString(key);
			R.data_type=QString(data_type);
			R.length_of_data=length_of_data;
			R.pos=filepos+length_of_header;
			m_records[QString(key)]=R;
			m_record_keys.append(QString(key));
			m_file->seek(filepos+length_of_header+compressed_length_of_data);
			filepos=m_file->pos();
		}
		else {
		}
	}
	return true;
}
bool JArchive::writeData(const QString &key,const QByteArray &data,bool compress,const QString &data_type) {
	QByteArray data2;
	if (compress) data2=qCompress(data);
	
	QString tmp=key;
	if (!d->m_group_prefix.isEmpty())
		tmp=d->m_group_prefix+"/"+key;
	QByteArray key2=tmp.toAscii();
	QByteArray data_type2=data_type.toAscii();
	qint32 length_of_header=4+4+4+4+4+key2.count()+4+4+data_type2.count();
	qint32 length_of_data=data.count();
	qint32 compressed_length_of_data=length_of_data;
	if (compress) {
		compressed_length_of_data=data2.count();
	}
	qint32 length_of_key=key2.count();
	qint32 is_compressed=0;
	if (compress) is_compressed=1;
	qint32 code=JARCHIVE_BEGIN_RECORD_CODE;
	qint32 length_of_data_type=data_type2.count();
	
	d->write_int32(code); //4
	d->write_int32(length_of_header); //4
	d->write_int32(compressed_length_of_data); //4
	d->write_int32(length_of_data); //4
	d->write_int32(length_of_key); //4
	d->write_bytes(key2); //length_of_key
	d->write_int32(is_compressed); //4
	d->write_int32(length_of_data_type); //4
	d->write_bytes(data_type2); //length_of_data_type
	if (compress) d->write_bytes(data2);
	else d->write_bytes(data);
	return true;
}
QByteArray JArchive::readData(const QString &key) {
	if (!d->m_file) return QByteArray();
	QString tmp=key;
	if (!d->m_group_prefix.isEmpty())
		tmp=d->m_group_prefix+"/"+key;
	QString key2=tmp;
	if (!d->m_records.contains(key2)) return "";
	JArchiveRecordInfo *R=&d->m_records[key2];
	d->m_file->seek(R->pos);
	QByteArray ret=d->m_file->read(R->compressed_length_of_data);
	if (R->is_compressed) {
		ret=qUncompress(ret);
	}
	return ret;
}
bool JArchive::contains(const QString &key) {
	QString tmp=key;
	if (!d->m_group_prefix.isEmpty())
		tmp=d->m_group_prefix+"/"+key;
	QString key2=tmp;
	return d->m_records.contains(key2);
}
bool JArchive::writeValue(const QString &key,const QVariant &value,bool compress) {
	//This is how we used to do it, in version 2
	/*
		if (value.type()==QVariant::StringList) {
			QStringList list=value.toStringList();
			QString str="StringList::";
			for (int i=0; i<list.count(); i++) {
				str+=QString("{%1}").arg(list[i].count());
				str+=list[i];
			}
			return writeValue(key,str);
		}
		else {
			QByteArray tmp=value.toString().toAscii();
			return writeData(key,tmp,false,"Value");
		}
	}*/
	
	{ //as of version 3
		if (value.type()==QVariant::StringList) {
			QByteArray tmp;
			tmp.append("StringList");
			tmp.append((char)0);
			QStringList val=value.toStringList();
			for (int ii=0; ii<val.count(); ii++) {
				tmp.append(val[ii].toAscii());
				if (ii+1<val.count()) tmp.append((char)0);				
			}
			return writeData(key,tmp,compress,"Value");
		}
		else if (value.type()==QVariant::Image) {
			QImage img=value.value<QImage>();
			QString img_format="JPG";
			//if (img.hasAlphaChannel()) img_format="PNG";
			img_format="PNG"; //for now, use only PNG, because it seems that the static MAC version of the rwclient cannot handle JPG.
			QByteArray tmp;
			tmp.append("Image::"+img_format);
			tmp.append((char)0);
 			QBuffer buffer(&tmp);
 			buffer.open(QIODevice::Append);
 			img.save(&buffer,img_format.toAscii().data(),99);
 			buffer.close();
 			return writeData(key,tmp,compress,"Value");
		}
		else if (value.type()==QVariant::ByteArray) {
			QByteArray data=value.toByteArray();
			QByteArray tmp;
			tmp.append("ByteArray");
			tmp.append((char)0);
			tmp.append(data);
			return writeData(key,tmp,compress,"Value");
		}
		else if (value.type()==QVariant::PointF) {
			QPointF vvv=value.toPointF();
			QByteArray tmp;
			tmp.append("PointF");
			tmp.append((char)0);
			tmp.append(QString("%1,%2").arg(vvv.x()).arg(vvv.y()).toAscii());
			return writeData(key,tmp,compress,"Value");
		}
		else if (value.type()==QVariant::DateTime) {
			//added on 1/11/2012 to handle milliseconds!
			QByteArray tmp;
			tmp.append("DateTime");
			tmp.append((char)0);
			tmp.append(value.toDateTime().toString("yyyy-MM-ddThh:mm:ss.zzz"));
			return writeData(key,tmp,compress,"Value");
		}
		else if (value.type()==QVariant::Time) {
			//added on 1/11/2012 to handle milliseconds!
			QByteArray tmp;
			tmp.append("Time");
			tmp.append((char)0);
			tmp.append(value.toTime().toString("hh:mm:ss.zzz"));
			return writeData(key,tmp,compress,"Value");
		}
		else {
			QByteArray tmp;
			tmp.append(value.typeName());
			tmp.append((char)0);
			tmp.append(value.toString().toAscii());
			return writeData(key,tmp,compress,"Value");
		}
	}
}
QVariant JArchive::readValue(const QString &key,const QVariant &defaultval) {
	if (!contains(key)) return defaultval;
	
	if (d->m_read_version_number<=2) {
		QVariant ret=QString(readData(key));
		QString str=ret.toString();
		if (str.mid(0,12)=="StringList::") {
			QStringList list;
			int i=12;
			while (i<str.count()) {
				int ind1=str.indexOf('{',i);
				int ind2=str.indexOf('}',i);
				if ((ind1>=0)&&(ind2>=0)) {
					int len=str.mid(ind1+1,ind2-ind1-1).toInt();
					list << str.mid(ind2+1,len);
					i=ind2+1+len;
				}
				else i=str.count();
			}
			return list;
		}
		else {	
			return ret;
		}
	}
	else { //version 3
		QByteArray ddd=readData(key);
		if (ddd.count()==0) return defaultval;
		
		//search for first zero
		int iii=0;
		while ((iii<ddd.count())&&(ddd[iii]!=(char)0)) iii++;
		if (iii>=ddd.count()) return defaultval;
		QString dtype=QString(ddd.mid(0,iii));
		if (dtype=="StringList") {
			QByteArray ddd2=ddd.mid(iii+1);
			QList<QByteArray> list=ddd2.split((char)0);
			QStringList ret;
			foreach (QString str,list) {
				ret << QString(str);
			}
			return ret;
		}
		else if (dtype.indexOf("Image::")==0) {
			QString img_format=dtype.mid(QString("Image::").count());
			QByteArray ddd2=ddd.mid(iii+1);
			QBuffer buffer(&ddd2);
			buffer.open(QIODevice::ReadOnly);
			QImage img;
			img.load(&buffer,img_format.toAscii().data());
			buffer.close();
			return img;
		}
		else if (dtype=="ByteArray") {
			return ddd.mid(iii+1);
		}
		else if (dtype=="PointF") {
			QByteArray ddd2=ddd.mid(iii+1);
			QStringList list=QString(ddd2).split(",");
			return QPointF(list.value(0).toFloat(),list.value(1).toFloat());
		}
		else if (dtype=="DateTime") {
			QByteArray ddd2=ddd.mid(iii+1);
			if (ddd2.contains(" ")) return QDateTime::fromString(ddd2);
			else return QDateTime::fromString(ddd2,"yyyy-MM-ddThh:mm:ss.zzz");
		}
		else if (dtype=="Time") {
			QByteArray ddd2=ddd.mid(iii+1);
			if (ddd2.contains(" ")) return QTime::fromString(ddd2);
			else return QTime::fromString(ddd2,"hh:mm:ss.zzz");
		}
		else return QString(ddd.mid(iii+1));
	}
}
void JArchive::setValue(const QString &key,const QVariant &value,bool compress) {
	writeValue(key,value,compress);
}
QVariant JArchive::value(const QString &key,const QVariant &defaultval) {
	return readValue(key,defaultval);
}
bool JArchive::addFile(const QString &key,const QString &filename,bool compress) {
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly)) return false;
	QByteArray tmp=f.readAll();
	f.close();
	return writeData(key,tmp,compress,QString("File[%1]").arg(QFileInfo(filename).suffix()));
}
bool JArchive::extractFile(const QString &key,const QString &filename) {
	QByteArray tmp=readData(key);
	QFile f(filename);
	if (!f.open(QIODevice::WriteOnly)) return false;
	f.write(tmp);
	f.close();
	return true;
}

void JArchivePrivate::write_int32(qint32 x) {
	if (!m_file) return;
	QByteArray buf;
	QDataStream out(&buf,QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	out << x;
	m_file->write(buf);
}
qint32 JArchivePrivate::read_int32() {
	if (!m_file) return 0;
	qint32 ret;
	QByteArray buf=m_file->read(4);
	QDataStream in(buf);
	in.setVersion(QDataStream::Qt_4_5);
	in >> ret;
	return ret;
}
QByteArray JArchivePrivate::read_bytes(qint32 num) {
	if (!m_file) return QByteArray();
	return m_file->read(num);
}
void JArchivePrivate::write_bytes(const QByteArray &X) {
	if (!m_file) return;
	m_file->write(X);
}
QStringList JArchive::allKeys() const {
	return d->m_record_keys;
}
QStringList JArchive::childKeys() const {
	QStringList ret;
	QSet<QString> already_found;
	foreach (QString key,d->m_record_keys) {
		QString str;
		if (d->m_group_prefix.isEmpty()) {
			str=key;
		}
		else {
			if (key.indexOf(d->m_group_prefix)==0) {
				str=key.mid(d->m_group_prefix.count()+1);
			}
		}
		if (!str.isEmpty()) {
			int ind0=str.indexOf("/");
			if (ind0>=0) {
				QString tmp=str.mid(0,ind0);
				if (!tmp.isEmpty()) {
					if (!already_found.contains(tmp)) {
						ret << tmp;
						already_found.insert(tmp);
					}
				}
			}
			else {
				if (!already_found.contains(str)) {
					ret << str;
					already_found.insert(str);
				}
			}
		}
	}
	return ret;
}
void JArchive::writeFloatArray(const QString &key,const QList<float> &X,bool compress) {
	QByteArray buf;
	QDataStream out(&buf,QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	qint32 ct=X.count();
	out << ct;
	for (int i=0; i<ct; i++) {
		float val=X[i];
		out << val;
	}
	writeData(key,buf,compress,"FloatArray");
}
void JArchive::writeFloatArray(const QString &key,const QVector<float> &X,bool compress) {
	QByteArray buf;
	QDataStream out(&buf,QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	qint32 ct=X.count();
	out << ct;
	for (int i=0; i<ct; i++) {
		float val=X[i];
		out << val;
	}
	writeData(key,buf,compress,"FloatArray");
}
QList<float> JArchive::readFloatArray(const QString &key) {
	QByteArray buf=readData(key);
	QDataStream in(buf);
	in.setVersion(QDataStream::Qt_4_5);
	qint32 ct;
	in >> ct;
	QList<float> ret;
	for (int i=0; i<ct; i++) {
		float val;
		in >> val;
		ret << val;
	}
	return ret;
}
void JArchive::writeFloatArray(const QString &key,float *X,qint32 N,bool compress) {
	QByteArray buf;
	QDataStream out(&buf,QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	qint32 ct=N;
	out << ct;
	for (int i=0; i<ct; i++) {
		float val=X[i];
		out << val;
	}
	writeData(key,buf,compress,"FloatArray");
}
void JArchive::writeInt16Array(const QString &key,qint16 *X,qint32 N,bool compress) {
	QByteArray buf;
	QDataStream out(&buf,QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	qint32 ct=N;
	out << ct;
	for (long i=0; i<ct; i++) {
		qint16 val=X[i];
		out << val;
	}
	writeData(key,buf,compress,"Int16Array");
}
void JArchive::writeUint16Array(const QString &key,quint16 *X,qint32 N,bool compress) {
	QByteArray buf;
	QDataStream out(&buf,QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	qint32 ct=N;
	out << ct;
	for (long i=0; i<ct; i++) {
		quint16 val=X[i];
		out << val;
	}
	writeData(key,buf,compress,"Uint16Array");
}

void JArchive::readFloatArray(const QString &key,float *X,qint32 N) {
	QByteArray buf=readData(key);
	QDataStream in(buf);
	in.setVersion(QDataStream::Qt_4_5);
	qint32 ct;
	in >> ct;
	if (ct>N) ct=N;
	for (int i=0; i<ct; i++) {
		float val;
		in >> val;
		X[i]=val;
	}
}
void JArchive::readInt16Array(const QString &key,qint16 *X,qint32 N) {
	QByteArray buf=readData(key);
	QDataStream in(buf);
	in.setVersion(QDataStream::Qt_4_5);
	qint32 ct;
	in >> ct;
	if (ct>N) ct=N;
	for (int i=0; i<ct; i++) {
		qint16 val;
		in >> val;
		X[i]=val;
	}
}
void JArchive::readUint16Array(const QString &key,quint16 *X,qint32 N) {
	QByteArray buf=readData(key);
	QDataStream in(buf);
	in.setVersion(QDataStream::Qt_4_5);
	qint32 ct;
	in >> ct;
	if (ct>N) ct=N;
	for (int i=0; i<ct; i++) {
		quint16 val;
		in >> val;
		X[i]=val;
	}
}

void JArchive::beginGroup(const QString &prefix,const QString &custom_type) {
	if ((d->m_open_mode==JARCHIVE_MODE_WRITE)||(d->m_open_mode==JARCHIVE_MODE_APPEND)) {
		if (!custom_type.isEmpty()) {
			writeData(prefix,"",false,custom_type);
		}
	}
	
	JArchivePrefix P;
	P.name=prefix;
	P.type=1; //group
	d->m_current_prefix_path << P;
	if (d->m_group_prefix.isEmpty())
		d->m_group_prefix=prefix;
	else
		d->m_group_prefix+="/"+prefix;
	
}
void JArchive::endGroup() {
	if (d->m_current_prefix_path.count()>0) {
		int ii=d->m_current_prefix_path.count()-1;
		if (d->m_current_prefix_path[ii].type!=1) {
			qWarning() << "Unexpected endGroup for JArchive. **";
			return;		
		}
		d->m_current_prefix_path.removeAt(ii);
	}
	else {
		qWarning() << "Unexpected endGroup for JArchive.";
		return;
	};
	int ind=d->m_group_prefix.lastIndexOf('/');
	if (ind>=0) d->m_group_prefix=d->m_group_prefix.mid(0,ind);
	else d->m_group_prefix="";
}
int JArchive::beginReadArray(const QString &prefix) {
	JArchivePrefix P;
	P.name=prefix;
	P.type=2; //array
	d->m_current_prefix_path << P;
	if (d->m_group_prefix.isEmpty())
		d->m_group_prefix=prefix;
	else
		d->m_group_prefix+="/"+prefix;
		
	return readValue("size").toInt();
}
void JArchive::beginWriteArray(const QString &prefix,int size) {
	JArchivePrefix P;
	P.name=prefix;
	P.type=2; //array
	d->m_current_prefix_path << P;
	if (d->m_group_prefix.isEmpty())
		d->m_group_prefix=prefix;
	else
		d->m_group_prefix+="/"+prefix;
		
	writeValue("size",size);
}
void JArchive::setArrayIndex(int i) {
	{ //end array index if needed
		int ii=d->m_current_prefix_path.count()-1;
		if (ii>=0) {
			if (d->m_current_prefix_path[ii].type==3) { //array index
				d->m_current_prefix_path.removeAt(ii);
				int ind=d->m_group_prefix.lastIndexOf('/');
				if (ind>=0) d->m_group_prefix=d->m_group_prefix.mid(0,ind);
				else d->m_group_prefix="";
			}
		}
	}
	if (d->m_current_prefix_path.count()==0) {
		qWarning() << "Unexpected setArrayIndex for JArchive. **";
		return;
	}
	else {
		if (d->m_current_prefix_path[d->m_current_prefix_path.count()-1].type!=2) { //array
			qWarning() << "Unexpected setArrayIndex for JArchive.";
			return;			
		}
	}
	QString str=QString("%1").arg(i);
	JArchivePrefix P;
	P.name=str;
	P.type=3; //array index
	d->m_current_prefix_path << P;
	if (d->m_group_prefix.isEmpty())
		d->m_group_prefix=str;
	else
		d->m_group_prefix+="/"+str;
}
void JArchive::endArray() {
	{ //end array index if needed
		int ii=d->m_current_prefix_path.count()-1;
		if (ii>=0) {
			if (d->m_current_prefix_path[ii].type==3) { //array index
				d->m_current_prefix_path.removeAt(ii);
				int ind=d->m_group_prefix.lastIndexOf('/');
				if (ind>=0) d->m_group_prefix=d->m_group_prefix.mid(0,ind);
				else d->m_group_prefix="";
			}
		}
	}
	if (d->m_current_prefix_path.count()>0) {
		int ii=d->m_current_prefix_path.count()-1;
		if (d->m_current_prefix_path[ii].type!=2) {
			qWarning() << "Unexpected endArray for JArchive. **";
			return;		
		}
		d->m_current_prefix_path.removeAt(ii);
	}
	else {
		qWarning() << "Unexpected endArray for JArchive.";
		return;
	};
	int ind=d->m_group_prefix.lastIndexOf('/');
	if (ind>=0) d->m_group_prefix=d->m_group_prefix.mid(0,ind);
	else d->m_group_prefix="";
}
QStringList JArchive::readStringList(const QString &key) {
	QByteArray buf=readData(key);
	QDataStream in(buf);
	in.setVersion(QDataStream::Qt_4_5);
	qint32 ct;
	in >> ct;
	QStringList ret;
	for (int i=0; i<ct; i++) {
		qint32 size;
		in >> size;
		QString val;
		quint8 c;
		for (int j=0; j<size; j++) {
			in >> c;
			val.append(QChar(c));
		}
		ret << val;
	}
	return ret;
}
void JArchive::writeStringList(const QString &key,const QStringList &X,bool compress) {
	QByteArray buf;
	QDataStream out(&buf,QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	qint32 ct=X.count();
	out << ct;
	for (int i=0; i<ct; i++) {
		qint32 size=X[i].count();
		out << size;
		for (int j=0; j<size; j++) {
			quint8 c=(X[i])[j].toAscii();
			out << c;
		}
	}
	writeData(key,buf,compress,"StringList");
}
void JArchive::writeDoubleArray(const QString &key,const QList<double> &X,bool compress) {
	QByteArray buf;
	QDataStream out(&buf,QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	qint32 ct=X.count();
	out << ct;
	for (int i=0; i<ct; i++) {
		double val=X[i];
		out << val;
	}
	writeData(key,buf,compress,"DoubleArray");
}
QList<double> JArchive::readDoubleArray(const QString &key) {
	QByteArray buf=readData(key);
	QDataStream in(buf);
	in.setVersion(QDataStream::Qt_4_5);
	qint32 ct;
	in >> ct;
	QList<double> ret;
	for (int i=0; i<ct; i++) {
		double val;
		in >> val;
		ret << val;
	}
	return ret;
}
void JArchive::readDoubleArray(const QString &key,double *X,qint32 N) {
	QByteArray buf=readData(key);
	QDataStream in(buf);
	in.setVersion(QDataStream::Qt_4_5);
	qint32 ct;
	in >> ct;
	if (ct>N) ct=N;
	QList<float> ret;
	for (int i=0; i<ct; i++) {
		double val;
		in >> val;
		X[i]=val;
	}
}
void JArchive::writeDoubleArray(const QString &key,double *X,qint32 N,bool compress) {
	QByteArray buf;
	QDataStream out(&buf,QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	qint32 ct=N;
	out << ct;
	for (int i=0; i<ct; i++) {
		double val=X[i];
		out << val;
	}
	writeData(key,buf,compress,"DoubleArray");
}
QString JArchive::dataType(const QString &key) const {
	QString tmp=key;
	if (!d->m_group_prefix.isEmpty())
		tmp=d->m_group_prefix+"/"+key;
	QString key2=tmp;
	if (!d->m_records.contains(key2)) return "";
	return d->m_records[key2].data_type;
}
QString JArchive::fileName() const {
	return d->m_file_name;
}
long JArchive::dataSize(const QString &key) {
	if (!d->m_file) return 0;
	QString tmp=key;
	if (!d->m_group_prefix.isEmpty())
		tmp=d->m_group_prefix+"/"+key;
	QString key2=tmp;
	if (!d->m_records.contains(key2)) return 0;
	JArchiveRecordInfo *R=&d->m_records[key2];
	return R->length_of_data;
}









