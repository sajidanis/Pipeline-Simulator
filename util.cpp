/**
 * This file is used to save and compile the source code given to us by the user
 * After simulating the program we will erase the compiled program
 */

#include "util.h"

Util::Util() {

}

void Util::saveFile(QString code){
    QFile file("./test.c");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << code;
        file.close();
        qDebug() << "File written successfully.";
    } else {
        qDebug() << "Failed to open the file for writing.";
    }
}

void Util::compile(QString fileName){
    QProcess gccProcess;
    QStringList gccArgs;
    gccArgs << fileName << "-o" << "./test";

    try{
        gccProcess.start("gcc", gccArgs);
        if(gccProcess.waitForFinished(-1)){

            QByteArray standardOutput = gccProcess.readAllStandardOutput();
            qDebug() << "Standard Output:" << standardOutput;

            QByteArray standardError = gccProcess.readAllStandardError();
            qDebug() << "Standard Error:" << standardError;
            if(!standardError.isEmpty()){
                QString error = "[-] Compile Error: ";
                error.append(standardError);
                throw CompileException(error);
            }
        }
    } catch (QProcess::ProcessError e){
        throw e;
    }
}
