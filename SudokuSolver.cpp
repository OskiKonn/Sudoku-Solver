
#include "SudokuSolver.h"
#include <iostream>
#include <tuple>
#include <QObject>
#include <QPushButton>
#include <QMessageBox>

#define ROWS    730
#define COLS    324

SudokuSolver::SudokuSolver(QWidget *parent)
    : QMainWindow(parent)
{   
    ui.setupUi(this);

    // Connecting signals
    connect(ui.solveBtn, &QPushButton::clicked, this, &SudokuSolver::startSolving);
    connect(ui.solveBtn, &QPushButton::clicked, this, &SudokuSolver::encodeInputMatrix);
    connect(ui.solveBtn, &QPushButton::clicked, algo, &SolverAlgorithm::createMatrix);
    connect(ui.solveBtn, &QPushButton::clicked, algo, [this]() {
            algo->solve(0);
            });
    connect(algo, &SolverAlgorithm::raiseSolvedFlag, this, &SudokuSolver::injectToSudoku);
    connect(&timer, &QTimer::timeout, this, &SudokuSolver::onError);
    connect(ui.resetBtn, &QPushButton::clicked, this, &SudokuSolver::resetBoard);

    connectOnEditSignals();
}

void SudokuSolver::startSolving()
{
    algo->moveToThread(thread);
    timer.setSingleShot(true);
    thread->start();
}

void SudokuSolver::encodeInputMatrix() {
    
    timer.start(5000);

    for (int i = 0; i < COLS; ++i)
        algo->inputMatrix[0][i] = 1;

    int row, col, block, cell;
    int val;
    int currRow = 1;

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {

            QString cellName = QString("_%1%2").arg(i).arg(j);
            QLineEdit* sudokuCell = findChild<QLineEdit*>(cellName);

            if (sudokuCell) {

                if (sudokuCell->text() != QString("")) {

                    val = sudokuCell->text().toInt();
                    row = i * 9 + (val - 1);
                    col = 81 + j * 9 + (val - 1);

                    block = 162 + (((i / 3) * 3) + (j / 3)) * 9 + (val - 1);
                    cell = 243 + i * 9 + j;

                    algo->inputMatrix[currRow][row] = 1;
                    algo->inputMatrix[currRow][col] = 1;
                    algo->inputMatrix[currRow][block] = 1;
                    algo->inputMatrix[currRow][cell] = 1;

                    algo->inputValues[currRow] = val;

                    ++currRow;
                }
                else {
                    for (int k = 1; k <= 9; ++k)
                    {
                        val = k;
                        row = i * 9 + (val - 1);
                        col = 81 + j * 9 + (val - 1);

                        block = 162 + (((i / 3) * 3) + (j / 3)) * 9 + (val - 1);;
                        cell = 243 + i * 9 + j;

                        algo->inputMatrix[currRow][row] = 1;
                        algo->inputMatrix[currRow][col] = 1;
                        algo->inputMatrix[currRow][block] = 1;
                        algo->inputMatrix[currRow][cell] = 1;

                        algo->inputValues[currRow] = val;

                        ++currRow;
                    }
                }
            }
        }
    }
    std::cout << "\n" << sizeof(algo->inputMatrix);
}

void SudokuSolver::injectToSudoku()
{
    result_ptr = &algo->result;

    for (SolverAlgorithm::Node* value : *result_ptr)
    {
        int c = 243;
        int column = 0;

        for (c; c <= 324; ++c)
        {
            if (algo->inputMatrix[value->rowID][c] == 1)
            {
                column = c;
                break;
            }
        }

        if (column)
        {
            int row, col;
            std::tie(row, col) = calculateCellXY(column);

            QString cellName = QString("_%1%2").arg(row).arg(col);
            QLineEdit* sudokuCell = findChild<QLineEdit*>(cellName);

            if (sudokuCell)
            {
                QString qVal = QString::number(algo->inputValues[value->rowID]);
                sudokuCell->setText(qVal);
            }
        }
    }

    algo->unlinkMatrix();
    deactivateButton();
    timer.stop();

}

std::tuple<int, int> SudokuSolver::calculateCellXY(const int &column)
{
    int cell;
    int row = 0, col = 0;
    
    cell = column - 243;

    row = cell / 9;
    col = cell % 9;

    return {row, col};
}

void SudokuSolver::deactivateButton()
{
    QPushButton* button = ui.solveBtn;

    if (algo->solved)
    {
        button->setEnabled(false);
        button->setStyleSheet(QString("background - color: #8e8e8e; \ncolor: black; "));
    }
}

void SudokuSolver::activateButton()
{
    if (algo->solved && ui.solveBtn->isEnabled() == false)
    {
        ui.solveBtn->setEnabled(true);
        ui.solveBtn->setStyleSheet(QString("background-color: #9bbf6b;\ncolor: #4a4a4a;"));
        algo->solved = false;
    }
}

void SudokuSolver::connectOnEditSignals()
{
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j)
        {
            QString cellName = QString("_%1%2").arg(i).arg(j);
            QLineEdit* sudokuCell = findChild<QLineEdit*>(cellName);

            if (sudokuCell)
            {
                QObject::connect(sudokuCell, &QLineEdit::textChanged, this, &SudokuSolver::activateButton);
            }
        }
    }
}

void SudokuSolver::onError()
{
    if (thread->isRunning())
    {
        thread->terminate();
        std::cout << "\n\nERROR: Za dlugi czas wykonywania!!!" << std::endl;
    }

    algo->unlinkMatrix();
    QMessageBox::critical(this, QString("Failed solving sudoku"), QString("Time limit exceeded, validate input data"), QMessageBox::Ok);
}

void SudokuSolver::resetBoard()
{
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j)
        {
            QString cellName = QString("_%1%2").arg(i).arg(j);
            QLineEdit* sudokuCell = findChild<QLineEdit*>(cellName);

            if (sudokuCell)
            {
                sudokuCell->setText(QString(""));
            }
        }
    }
}

SudokuSolver::~SudokuSolver()
{
    delete algo;
}
