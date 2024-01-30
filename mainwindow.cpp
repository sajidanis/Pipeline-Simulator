#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "util.h"
#include "gdbprocess.h"
#include "pipelineutil.h"
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QVector>

GdbProcess *GDB_PROCESS = new GdbProcess();
PipelineUtil pipeUtil;

long IF, ID, EXE, MEM, WB;

static int current_row = 0;

static QVector<Instruction> disCode = {};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    GDB_PROCESS->close();
    delete ui;
}


void MainWindow::on_submitButton_clicked()
{
    ui->textBrowser_4->clear();
    QString code = ui->textEdit->toPlainText();

    Util::saveFile(code);

    // Compile the saved file
    try{
        Util::compile("./test.c");
    } catch(CompileException ex){
        ui->textBrowser_4->setText(QString(ex.getMessage())); // Print the message on the status
        return;
    }

    // Run the compiled code through gdb and decompile the functions


    GDB_PROCESS->run();
    QVector<Instruction> instructions = GDB_PROCESS->disassembleProgram();
    disCode = instructions;

    ui->decompiler->setColumnCount(3);
    QStringList assemblyHeaders = {"Address", "Opcode", "Mnemonic"};
    ui->decompiler->setHorizontalHeaderLabels(assemblyHeaders);

    instructions.removeFirst();
    ui->decompiler->setRowCount(static_cast<int>(instructions.size()));
    for (int row = 0; row < instructions.size(); ++row) {
        ui->decompiler->setItem(row, 0, new QTableWidgetItem(instructions[row].address));
        ui->decompiler->setItem(row, 1, new QTableWidgetItem(instructions[row].opcode));
        ui->decompiler->setItem(row, 2, new QTableWidgetItem(instructions[row].mnemonic));
    }
    ui->decompiler->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Get the current state of registers
    QVector<Register> reg = GDB_PROCESS->getRegInfo();;

    ui->registerTable->setColumnCount(3);
    QStringList registerHeaders = {"Name", "Hex Value", "Value"};
    ui->registerTable->setHorizontalHeaderLabels(registerHeaders);

    ui->registerTable->setRowCount(static_cast<int>(reg.size()));
    for (int row = 0; row < reg.size(); ++row) {
        ui->registerTable->setItem(row, 0, new QTableWidgetItem(reg[row].name));
        ui->registerTable->setItem(row, 1, new QTableWidgetItem(reg[row].hexValue));
        ui->registerTable->setItem(row, 2, new QTableWidgetItem(reg[row].desc));
    }
    ui->registerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Finding the address for main --> current instruction to execute

    QString rip;
    for(auto &el : reg){
        if(el.name == "rip"){
            rip = el.hexValue;
        }
    }
    ui->tabWidget->setCurrentIndex(1);
    bool ok;
    // Highlight that row in disassemble
    int rowSize = ui->decompiler->rowCount();
    for (int row = 0; row < rowSize; ++row) {
        QTableWidgetItem *currentItem = ui->decompiler->item(row, 0); // 0th column is address column
        if (currentItem && currentItem->text().sliced(0, 18).toLongLong(&ok, 16) == rip.toLongLong(&ok, 16)) {
            // Activate (select) the entire row
            ui->decompiler->setCurrentItem(currentItem);
            ui->decompiler->selectRow(row);
            current_row = row+1;
            ui->decompiler->scrollToItem(currentItem);
            break;
        }
    }
}


void MainWindow::on_stepButton_clicked()
{
    QVector<QTableWidgetItem*> pipelinePhases;
    QTableWidgetItem *IF_cell = ui->tableWidget->item(0, 0);
    QTableWidgetItem *ID_cell = ui->tableWidget->item(0, 1);
    QTableWidgetItem *EXE_cell = ui->tableWidget->item(0, 2);
    QTableWidgetItem *MEM_cell = ui->tableWidget->item(0, 3);
    QTableWidgetItem *WB_cell = ui->tableWidget->item(0, 4);
    pipelinePhases.append(IF_cell);
    pipelinePhases.append(ID_cell);
    pipelinePhases.append(EXE_cell);
    pipelinePhases.append(MEM_cell);
    pipelinePhases.append(WB_cell);

    current_row = pipeUtil.nextClock(disCode, pipelinePhases, current_row, ui->textBrowser_4, GDB_PROCESS);
    current_row++;

}

void MainWindow::on_tableWidget_cellClicked(int row, int column)
{

}

