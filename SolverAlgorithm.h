#pragma once
#include <vector>
#include <QObject>

#define ROWS 730
#define COLS 324

class SolverAlgorithm : public QObject {

    Q_OBJECT

public:
    struct Node {

        Node* left;
        Node* right;
        Node* up;
        Node* down;
        Node* column;
        int rowID;
        int columnID;
        int nodeCount;

    };

    bool** inputMatrix = new bool* [ROWS];
    bool solved = false;
    int inputValues[729];
    std::vector<Node*> result;

    SolverAlgorithm();
    int getRightNode(int);
    int getUpNode(int);
    int getDownNode(int);
    int getLeftNode(int);
    void remove(Node* target);
    void restore(Node* target);
    void printResult();
    void unlinkMatrix();
    Node* getMinNodesColumn();
    ~SolverAlgorithm();

public slots:
    Node* createMatrix();
    void solve(int k);

signals:
    void raiseSolvedFlag();

private:
    struct Node** Matrix = new Node * [ROWS];
    struct Node* header = new Node();

};