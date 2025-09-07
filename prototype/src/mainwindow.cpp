#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setWindowTitle("Blood Gas Analyzer v1.0");
    resize(800, 600);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    
    setupInputSection();
    setupResultsSection();
    
    // Connect signals
    connect(analyzeButton, &QPushButton::clicked, this, &MainWindow::analyzeBloodGas);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearInputs);
}

void MainWindow::setupInputSection()
{
    inputGroup = new QGroupBox("Blood Gas Values", this);
    inputLayout = new QGridLayout(inputGroup);
    
    // Create input fields
    inputLayout->addWidget(new QLabel("pH:"), 0, 0);
    pHEdit = new QLineEdit(this);
    pHEdit->setPlaceholderText("7.35 - 7.45");
    inputLayout->addWidget(pHEdit, 0, 1);
    
    inputLayout->addWidget(new QLabel("pCO2 (mmHg):"), 1, 0);
    pCO2Edit = new QLineEdit(this);
    pCO2Edit->setPlaceholderText("35 - 45");
    inputLayout->addWidget(pCO2Edit, 1, 1);
    
    inputLayout->addWidget(new QLabel("pO2 (mmHg):"), 2, 0);
    pO2Edit = new QLineEdit(this);
    pO2Edit->setPlaceholderText("> 80");
    inputLayout->addWidget(pO2Edit, 2, 1);
    
    inputLayout->addWidget(new QLabel("HCO3- (mEq/L):"), 0, 2);
    hco3Edit = new QLineEdit(this);
    hco3Edit->setPlaceholderText("22 - 28");
    inputLayout->addWidget(hco3Edit, 0, 3);
    
    inputLayout->addWidget(new QLabel("Base Excess (mEq/L):"), 1, 2);
    baseExcessEdit = new QLineEdit(this);
    baseExcessEdit->setPlaceholderText("-2 to +2");
    inputLayout->addWidget(baseExcessEdit, 1, 3);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    analyzeButton = new QPushButton("Analyze", this);
    clearButton = new QPushButton("Clear", this);
    buttonLayout->addWidget(analyzeButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();
    
    inputLayout->addLayout(buttonLayout, 3, 0, 1, 4);
    
    mainLayout->addWidget(inputGroup);
}

void MainWindow::setupResultsSection()
{
    resultsGroup = new QGroupBox("Analysis Results", this);
    resultsLayout = new QVBoxLayout(resultsGroup);
    
    // Results table
    resultsTable = new QTableWidget(0, 3, this);
    resultsTable->setHorizontalHeaderLabels(QStringList() << "Parameter" << "Value" << "Status");
    resultsTable->horizontalHeader()->setStretchLastSection(true);
    resultsTable->setAlternatingRowColors(true);
    resultsLayout->addWidget(resultsTable);
    
    // Interpretation label
    interpretationLabel = new QLabel("Enter blood gas values and click 'Analyze' to see results.", this);
    interpretationLabel->setWordWrap(true);
    interpretationLabel->setStyleSheet("QLabel { padding: 10px; border: 1px solid gray; background-color: grey; }");
    resultsLayout->addWidget(interpretationLabel);
    
    mainLayout->addWidget(resultsGroup);
}

void MainWindow::analyzeBloodGas()
{
    try {
        // Get values from input fields
        double pH = pHEdit->text().toDouble();
        double pCO2 = pCO2Edit->text().toDouble();
        double pO2 = pO2Edit->text().toDouble();
        double hco3 = hco3Edit->text().toDouble();
        double baseExcess = baseExcessEdit->text().toDouble();
        
        // Basic validation
        if (pH < 6.8 || pH > 8.0 || pCO2 <= 0 || pO2 <= 0 || hco3 <= 0) {
            QMessageBox::warning(this, "Invalid Input", "Please enter valid blood gas values.");
            return;
        }
        
        // Set values in analyzer
        analyzer.setPH(pH);
        analyzer.setPCO2(pCO2);
        analyzer.setPO2(pO2);
        analyzer.setHCO3(hco3);
        analyzer.setBaseExcess(baseExcess);
        
        // Perform analysis
        analyzer.analyze();
        
        // Update results table
        resultsTable->setRowCount(5);
        
        resultsTable->setItem(0, 0, new QTableWidgetItem("pH"));
        resultsTable->setItem(0, 1, new QTableWidgetItem(QString::number(pH, 'f', 2)));
        resultsTable->setItem(0, 2, new QTableWidgetItem(analyzer.getPHStatus()));
        
        resultsTable->setItem(1, 0, new QTableWidgetItem("pCO2 (mmHg)"));
        resultsTable->setItem(1, 1, new QTableWidgetItem(QString::number(pCO2, 'f', 1)));
        resultsTable->setItem(1, 2, new QTableWidgetItem(analyzer.getPCO2Status()));
        
        resultsTable->setItem(2, 0, new QTableWidgetItem("pO2 (mmHg)"));
        resultsTable->setItem(2, 1, new QTableWidgetItem(QString::number(pO2, 'f', 1)));
        resultsTable->setItem(2, 2, new QTableWidgetItem(analyzer.getPO2Status()));
        
        resultsTable->setItem(3, 0, new QTableWidgetItem("HCO3- (mEq/L)"));
        resultsTable->setItem(3, 1, new QTableWidgetItem(QString::number(hco3, 'f', 1)));
        resultsTable->setItem(3, 2, new QTableWidgetItem(analyzer.getHCO3Status()));
        
        resultsTable->setItem(4, 0, new QTableWidgetItem("Base Excess"));
        resultsTable->setItem(4, 1, new QTableWidgetItem(QString::number(baseExcess, 'f', 1)));
        resultsTable->setItem(4, 2, new QTableWidgetItem(analyzer.getBaseExcessStatus()));
        
        // Update interpretation
        interpretationLabel->setText(analyzer.getInterpretation());
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Analysis Error", QString("Error during analysis: %1").arg(e.what()));
    }
}

void MainWindow::clearInputs()
{
    pHEdit->clear();
    pCO2Edit->clear();
    pO2Edit->clear();
    hco3Edit->clear();
    baseExcessEdit->clear();
    
    resultsTable->setRowCount(0);
    interpretationLabel->setText("Enter blood gas values and click 'Analyze' to see results.");
}
