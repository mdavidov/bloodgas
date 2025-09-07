#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include "bloodgasanalyzer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void analyzeBloodGas();
    void clearInputs();

private:
    void setupUI();
    void setupInputSection();
    void setupResultsSection();
    
    // UI Elements
    QWidget* centralWidget;
    QVBoxLayout* mainLayout;

    // Input section
    QGroupBox* inputGroup;
    QGridLayout* inputLayout;
    QLineEdit* pHEdit;
    QLineEdit* pCO2Edit;
    QLineEdit* pO2Edit;
    QLineEdit* hco3Edit;
    QLineEdit* baseExcessEdit;
    QPushButton* analyzeButton;
    QPushButton* clearButton;

    // Results section
    QGroupBox* resultsGroup;
    QVBoxLayout* resultsLayout;
    QTableWidget* resultsTable;
    QLabel* interpretationLabel;

    // Blood gas analyzer
    BloodGasAnalyzer analyzer;
};
