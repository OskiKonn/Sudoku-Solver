#include "SolverAlgorithm.h"
#include <iostream>
#include <vector>

#define ROWS 730
#define COLS 324

SolverAlgorithm::SolverAlgorithm() {
    for (int i = 0; i < ROWS; ++i) {
        inputMatrix[i] = new bool[COLS];
        std::fill(inputMatrix[i], inputMatrix[i] + COLS, false);
    }
    for (int i = 0; i < ROWS; ++i) {
        Matrix[i] = new Node[COLS];
    }
}


int SolverAlgorithm::getRightNode(int index) {
    return (index > COLS - 2) ? 0 : index + 1;
}

int SolverAlgorithm::getLeftNode(int index) {
    return (index < 1) ? COLS - 1 : index - 1;
}

int SolverAlgorithm::getUpNode(int index) {
        return (index < 1) ? ROWS - 1 : index - 1;
}

int SolverAlgorithm::getDownNode(int index) {
    return (index > ROWS - 2) ? 0 : index + 1;
}

SolverAlgorithm::Node *SolverAlgorithm::createMatrix() {

    int rows = 729, cols = 324;

    for (int i = 0; i <= rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (inputMatrix[i][j]) {

                if (i)
                    Matrix[0][j].nodeCount++;

                Matrix[i][j].column = &Matrix[0][j];
                Matrix[i][j].rowID = i;
                Matrix[i][j].columnID = j;

                int a, b;

                a = i, b = j;

                do { a = getDownNode(a); } while (!inputMatrix[a][b] && a != i);
                Matrix[i][j].down = &Matrix[a][j];

                a = i, b = j;

                do { a = getUpNode(a); } while (!inputMatrix[a][b] && a != i);
                Matrix[i][j].up = &Matrix[a][j];

                a = i, b = j;

                do { b = getLeftNode(b); } while (!inputMatrix[a][b] && b != j);
                Matrix[i][j].left = &Matrix[i][b];

                a = i, b = j;

                do { b = getRightNode(b); } while (!inputMatrix[a][b] && b != j);
                Matrix[i][j].right = &Matrix[i][b];

            }
        }
    }

    header->right = &Matrix[0][0];
    Matrix[0][0].left = header;

    header->left = &Matrix[0][COLS - 1];
    Matrix[0][COLS - 1].right = header;

    return header;
}

void SolverAlgorithm::remove(Node* targetNode) {
    
    Node* column = targetNode->column;
    Node* rowNode;
    Node* rightNode;

    column->left->right = column->right;
    column->right->left = column->left;

    for (rowNode = column->down; rowNode != column; rowNode = rowNode->down)
    {
        for (rightNode = rowNode->right; rightNode != rowNode; rightNode = rightNode->right)
        {
            rightNode->up->down = rightNode->down;
            rightNode->down->up = rightNode->up;

            --Matrix[0][rightNode->columnID].nodeCount;
        }
    }
}

void SolverAlgorithm::restore(Node* targetNode)
{
    Node* column = targetNode->column;
    Node* rowNode, * rightNode;

    column->left->right = column;
    column->right->left = column;

    for (rowNode = column->down; rowNode != column; rowNode = rowNode->down)
    {
        for (rightNode = rowNode->right; rightNode != rowNode; rightNode = rightNode->right)
        {
            rightNode->up->down = rightNode;
            rightNode->down->up = rightNode;

            ++Matrix[0][rightNode->columnID].nodeCount;
        }
    }
}

SolverAlgorithm::Node* :: SolverAlgorithm::getMinNodesColumn()
{
    int n = 0;
    Node* head = header;
    Node* minCol = head->right;

    head = head->right->right;

    do {
        if (head->nodeCount < minCol->nodeCount)
        {
            minCol = head;
        }

        head = head->right;
        ++n;
    } while (head != header);

    return minCol;
}

void SolverAlgorithm::printResult()
{
    std::cout << "\nRozwiazanie: ";
    std::vector<Node*>::iterator it;

    for (it = result.begin(); it != result.end(); ++it)
    {
        std::cout << (*it)->rowID << ": ";
        std::cout << inputValues[(*it)->rowID];
        std::cout << "\n";
    }
    emit raiseSolvedFlag();
}

void SolverAlgorithm::solve(int k)
{

    if (header->right == header) {
        printResult();
        solved = true;
        return;
    }


    Node* column, * rowNode, * rightNode, * leftNode;
    column = getMinNodesColumn();

    remove(column);

    for (rowNode = column->down; rowNode != column; rowNode = rowNode->down)
    {

        result.push_back(rowNode);

        for (rightNode = rowNode->right; rightNode != rowNode; rightNode = rightNode->right)
        {
            remove(rightNode);
        }

        solve(k + 1);

        if (solved)
            return;

        result.pop_back();

        column = rowNode->column;

        for (leftNode = rowNode->right; leftNode != rowNode; leftNode = leftNode->right)
        {
            restore(leftNode);
        }

    }

    restore(column);
}

void SolverAlgorithm::unlinkMatrix()
{
    Node* column = header->right;

    while (column != header) {
        
        Node* row = column->down;
        Node* nextColumn = column->right;

        while (row != column) {
            Node* nextRow = row->down;
            row->up = nullptr;
            row->down = nullptr;
            row->left = nullptr;
            row->right = nullptr;
            row = nextRow;
        }
        
        column->left = nullptr;
        column->right = nullptr;
        column->up = nullptr;
        column->down = nullptr;
        column = nextColumn;
    }

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j)
        {
            inputMatrix[i][j] = false;
        }
    }

    for (int i = 0; i < ROWS - 1; ++i) {
            inputValues[i] = false;
    }

    while (result.size() > 0) {
        result.pop_back();
    }
}

SolverAlgorithm::~SolverAlgorithm()
{
    for (int i = 0; i < 9; ++i) {
        delete[] inputMatrix[i];
    }
    
    delete[] inputMatrix;

    for (int i = 0; i < ROWS; ++i) {
        delete[] Matrix[i];
    }

    delete[] Matrix;
    delete header;
}