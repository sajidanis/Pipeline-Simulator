#include "gdbprocess.h"


GdbProcess::GdbProcess(){

}

QString GdbProcess::send_command(QString command){
    this->gdbProcess.write(command.toStdString().c_str());
    this->gdbProcess.waitForReadyRead();
    return gdbProcess.readAllStandardOutput();
}

void GdbProcess::initialize(){
    QStringList gdbArgs;
    gdbArgs << "./test";
    gdbProcess.start("gdb", gdbArgs);
    qInfo("Started GDB Process");
}

void GdbProcess::run(){
    initialize();
    send_command("break main\n"); // Set a breakpoint at the 'main' function
    send_command("run\n");
    while(gdbProcess.waitForReadyRead(1000)){
        gdbProcess.readAllStandardOutput();
    }
    qInfo() << gdbProcess.readAll();
}

QList<Functions> GdbProcess::getFunctionList(){
    QString functions = send_command("info functions\n");
    while(gdbProcess.waitForReadyRead(2000)){
        gdbProcess.readAllStandardOutput();
    }
    QStringList lines = functions.split('\n');
    QList<Functions> functionList;
    for (const QString &line : lines) {
        if (line.startsWith("0x")) {
            // Extract function address and name
            QStringList parts = line.split("  ");
            if (parts.length() > 1) {
                Functions f;
                f.functionName = parts.at(1);
                f.functionAddr = parts.at(0);
                functionList.append(f);
                if(f.functionName == "_fini"){
                    break;
                }
            }
        }
    }
    return functionList;
}

QVector<Instruction> GdbProcess::extractInstructions(const QString& objdumpOutput) {
    QVector<Instruction> instructions;
    QStringList lines = objdumpOutput.split('\n');
    for (const QString &line : lines) {
        if (line.startsWith("   0x") || line.startsWith("=> 0x")) {
            // Extract function address and name
            QStringList parts = line.split("\t");
            if (parts.length() >= 3) {
                Instruction instruction;
                instruction.address = parts[0].sliced(3);
                instruction.opcode = parts[1];
                instruction.mnemonic = parts[2];
                instructions.append(instruction);
            }
        }
    }
    return instructions;
}

QVector<Instruction> GdbProcess::disassembleProgram(){
    QList<Functions> functionList = getFunctionList();
    QString disasses_command = "disassemble /r ";
    bool ok;
    QString last_addr = "0x";
    last_addr.append(QString::number(functionList.last().functionAddr.toULongLong(&ok, 16) + 12, 16));

    disasses_command.append(functionList[0].functionAddr);
    disasses_command.append(", ");
    disasses_command.append(last_addr);
    disasses_command.append("\n");

    QString disassembled_code = send_command(disasses_command);
    while(gdbProcess.waitForReadyRead(1000)){
        disassembled_code.append(gdbProcess.readAllStandardOutput());
    }

    QVector<Instruction> instructions = extractInstructions(disassembled_code);

    return instructions;
}

QVector<Register> GdbProcess::extractReg(const QString& regOutput){
    QList<Register> registerList;
    QStringList tokens = regOutput.split("\n");
    static QRegularExpression registerRegex(R"((\w+)\s+(\w+)\s+(.+))");
    foreach (const QString& line, tokens) {
        QRegularExpressionMatch match = registerRegex.match(line);
        if (line.contains(registerRegex)) {
            Register registerInfo;
            registerInfo.name = match.captured(1);
            registerInfo.hexValue = match.captured(2);
            registerInfo.desc = match.captured(3);
            registerList.append(registerInfo);
        }
    }
    return registerList;
}

void GdbProcess::runNextInstruction(){
    send_command("si\n");
}


QVector<Register> GdbProcess::getRegInfo(){
    QString reg_data = send_command("info r\n");
    return this->extractReg(reg_data);
}

QMap<QString, QString> GdbProcess::getRegData(){
    QString regOutput = send_command("info r\n");
    QMap<QString, QString> registerList;
    QStringList tokens = regOutput.split("\n");
    static QRegularExpression registerRegex(R"((\w+)\s+(\w+)\s+(.+))");
    foreach (const QString& line, tokens) {
        QRegularExpressionMatch match = registerRegex.match(line);
        if (line.contains(registerRegex)) {
            Register registerInfo;
            registerInfo.name = match.captured(1);
            registerInfo.hexValue = match.captured(2);
            registerInfo.desc = match.captured(3);
            registerList[registerInfo.name] = registerInfo.hexValue;
        }
    }
    return registerList;
}

void GdbProcess::close(){
    this->gdbProcess.close();
}
