#ifndef GDBPROCESS_H
#define GDBPROCESS_H

#include <QDebug>
#include <QList>
#include <QProcess>
#include <QString>
#include <QVector>
#include <QRegularExpression>
#include <QRegularExpressionMatch>


struct Functions{
    QString functionName;
    QString functionAddr;
};

struct Instruction {
    QString address;
    QString opcode;
    QString mnemonic;
};


struct Register{
    QString name;
    QString hexValue;
    QString desc;
};


class GdbProcess
{
private:
    QString send_command(QString command);
    QList<Functions> getFunctionList();
    QProcess gdbProcess;
    QVector<Instruction> extractInstructions(const QString &objdumpOutput);
    QVector<Register> extractReg(const QString &regOutput);

public:
    GdbProcess();
    void initialize();
    QVector<Instruction> disassembleProgram();
    void run();
    void close();
    QVector<Register> getRegInfo();
    QMap<QString, QString> getRegData();
    void runNextInstruction();
};

#endif // GDBPROCESS_H
