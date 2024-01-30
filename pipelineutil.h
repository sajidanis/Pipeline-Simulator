#ifndef PIPELINEUTIL_H
#define PIPELINEUTIL_H
#include "gdbprocess.h"
#include <QTableWidgetItem>
#include <QTextBrowser>
#include <QMap>

class PipelineUtil
{
public:
    PipelineUtil();
    Instruction fetchInstructions(QVector<Instruction> disCode, int current_row);
    long nextClock(QVector<Instruction>, QVector<QTableWidgetItem *>, int, QTextBrowser*, GdbProcess*);
};

#endif // PIPELINEUTIL_H
