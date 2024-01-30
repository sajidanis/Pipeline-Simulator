#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QDebug>
#include <QErrorMessage>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QException>

class Util
{
public:
    Util();
    void static saveFile(QString code);
    void static compile(QString fileName);
};

class CompileException: public QException {
private:
    QString message;

public :
    CompileException(QString const& msg = "") throw() : message(msg) {}
    void raise() const override { throw *this; }
    CompileException *clone() const override { return new CompileException(*this);}
    QString getMessage() const {return this->message;}
};

#endif // UTIL_H
