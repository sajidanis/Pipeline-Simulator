#include "pipelineutil.h"

PipelineUtil::PipelineUtil()
{

}

Instruction PipelineUtil::fetchInstructions(QVector<Instruction> disCode, int current_row){
    return disCode[current_row];
}

bool validateInstructionc(QString decode, QString rip){
    bool ok;
    return decode.sliced(0, 18).toLongLong(&ok, 16) == rip.toLongLong(&ok, 16);
}

long getCurrentRow(QVector<Instruction> dis, QString rip){
    // Implement by binaray search { TODO }
    bool ok;
    qInfo() << dis[88].address.sliced(0, 18);
    for(int row = 0 ; row < dis.size() ; row++){
        if(dis[row].address.sliced(0, 18).toLongLong(&ok, 16) == rip.toLongLong(&ok, 16)){
            return row;
        }
    }
}

long PipelineUtil::nextClock(QVector<Instruction> dis, QVector<QTableWidgetItem *> pipe, int current_row, QTextBrowser *tb, GdbProcess *gdb){
    QString IF_data = "[+] IF STAGE => \t";
    QString ID_data = "[+] ID STAGE => \t";
    QString EXE_data = "[+] EXE STAGE => \t";
    QString MEM_data = "[+] MEM STAGE => \t";
    QString WB_data = "[+] WB STAGE => \t";
    tb->clear();
    qInfo() << "into next clock";
    if(pipe[3]->text() != "0"){
        pipe[4]->setText(pipe[3]->text());
        WB_data.append(dis[pipe[4]->text().toInt()].opcode + "\t --> (" +dis[pipe[4]->text().toInt()].mnemonic + ")");
    }
    if(pipe[2]->text() != "0"){
        // execute cycle and check for rip for any branched data
        gdb->runNextInstruction();
        pipe[3]->setText(pipe[2]->text());
        MEM_data.append(dis[pipe[3]->text().toInt()].opcode + "\t --> ( " +dis[pipe[3]->text().toInt()].mnemonic + " )");
    }
    // current instructions has been executed
    // Now we have to validate the fetch and decode pipelined instructions for their validity
    QMap<QString, QString> regData = gdb->getRegData();
    qInfo() << "into next clock";
    if(pipe[1]->text() != "0" && !validateInstructionc(dis[pipe[1]->text().toInt()].address, regData["rip"])){
        pipe[2]->setText("0");
        pipe[1]->setText("0");
        pipe[0]->setText("0");
        current_row = getCurrentRow(dis, regData["rip"]);
    }

    if(pipe[1]->text() != "0"){
        pipe[2]->setText(pipe[1]->text());
        EXE_data.append(dis[pipe[2]->text().toInt()].opcode + "\t --> ( " +dis[pipe[2]->text().toInt()].mnemonic + " )");
    }
    if(pipe[0]->text() != "0"){
        pipe[1]->setText(pipe[0]->text());
        ID_data.append(dis[pipe[1]->text().toInt()].opcode + "\t --> ( " +dis[pipe[1]->text().toInt()].mnemonic + " )");
    }

    pipe[0]->setText(QString::number(current_row));

    IF_data.append(dis[current_row].opcode + "\t --> ( " +dis[current_row].mnemonic + " )");
    tb->append(IF_data);
    tb->append(ID_data);
    tb->append(EXE_data);
    tb->append(MEM_data);
    tb->append(WB_data);

    return current_row;
}

