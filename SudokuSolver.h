#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SudokuSolver.h"
#include "SolverAlgorithm.h"
#include <QLineEdit>
#include <QDebug>
#include <vector>
#include <QThread>
#include <QTimer>

class SudokuSolver : public QMainWindow
{
    Q_OBJECT

public:
    QThread *thread = new QThread;
    QTimer timer;

    SudokuSolver(QWidget *parent = nullptr);
    ~SudokuSolver();

public slots:
    void encodeInputMatrix();
    void injectToSudoku();
    void activateButton();
    void deactivateButton();
    void onError();

private:
    SolverAlgorithm *algo = new SolverAlgorithm();
    Ui::SudokuSolverClass ui;
    std::vector<SolverAlgorithm::Node*> *result_ptr;

    std::tuple<int, int> calculateCellXY(const int&);

    void connectOnEditSignals();
    void startSolving();
    void resetBoard();
};
