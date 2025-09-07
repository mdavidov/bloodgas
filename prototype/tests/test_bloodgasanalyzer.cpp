#include <QtTest/QtTest>
#include "../src/bloodgasanalyzer.h"

class BloodGasAnalyzerTest : public QObject
{
    Q_OBJECT

private slots:
    void testNormalValues();
    void testRespiratoryAcidosis();
    void testMetabolicAcidosis();
    void testRespiratoryAlkalosis();
    void testMetabolicAlkalosis();
    void testHypoxemia();
};

void BloodGasAnalyzerTest::testNormalValues()
{
    BloodGasAnalyzer analyzer;
    analyzer.setPH(7.40);
    analyzer.setPCO2(40);
    analyzer.setPO2(95);
    analyzer.setHCO3(24);
    analyzer.setBaseExcess(0);
    
    analyzer.analyze();
    
    QCOMPARE(analyzer.getPHStatus(), QString("Normal"));
    QCOMPARE(analyzer.getPCO2Status(), QString("Normal"));
    QCOMPARE(analyzer.getPO2Status(), QString("Normal"));
    QCOMPARE(analyzer.getHCO3Status(), QString("Normal"));
    QCOMPARE(analyzer.getBaseExcessStatus(), QString("Normal"));
    QVERIFY(analyzer.getInterpretation().contains("Normal pH"));
}

void BloodGasAnalyzerTest::testRespiratoryAcidosis()
{
    BloodGasAnalyzer analyzer;
    analyzer.setPH(7.25);
    analyzer.setPCO2(55);
    analyzer.setPO2(90);
    analyzer.setHCO3(24);
    analyzer.setBaseExcess(0);
    
    analyzer.analyze();
    
    QCOMPARE(analyzer.getPHStatus(), QString("Low (Acidic)"));
    QCOMPARE(analyzer.getPCO2Status(), QString("High"));
    QVERIFY(analyzer.getInterpretation().contains("Respiratory Acidosis"));
}

void BloodGasAnalyzerTest::testMetabolicAcidosis()
{
    BloodGasAnalyzer analyzer;
    analyzer.setPH(7.25);
    analyzer.setPCO2(30);
    analyzer.setPO2(95);
    analyzer.setHCO3(18);
    analyzer.setBaseExcess(-8);
    
    analyzer.analyze();
    
    QCOMPARE(analyzer.getPHStatus(), QString("Low (Acidic)"));
    QCOMPARE(analyzer.getHCO3Status(), QString("Low"));
    QCOMPARE(analyzer.getBaseExcessStatus(), QString("Negative (Base Deficit)"));
    QVERIFY(analyzer.getInterpretation().contains("Metabolic Acidosis"));
}

void BloodGasAnalyzerTest::testRespiratoryAlkalosis()
{
    BloodGasAnalyzer analyzer;
    analyzer.setPH(7.50);
    analyzer.setPCO2(28);
    analyzer.setPO2(100);
    analyzer.setHCO3(22);
    analyzer.setBaseExcess(0);
    
    analyzer.analyze();
    
    QCOMPARE(analyzer.getPHStatus(), QString("High (Alkaline)"));
    QCOMPARE(analyzer.getPCO2Status(), QString("Low"));
    QVERIFY(analyzer.getInterpretation().contains("Respiratory Alkalosis"));
}

void BloodGasAnalyzerTest::testMetabolicAlkalosis()
{
    BloodGasAnalyzer analyzer;
    analyzer.setPH(7.50);
    analyzer.setPCO2(45);
    analyzer.setPO2(95);
    analyzer.setHCO3(32);
    analyzer.setBaseExcess(8);
    
    analyzer.analyze();
    
    QCOMPARE(analyzer.getPHStatus(), QString("High (Alkaline)"));
    QCOMPARE(analyzer.getHCO3Status(), QString("High"));
    QCOMPARE(analyzer.getBaseExcessStatus(), QString("Positive (Base Excess)"));
    QVERIFY(analyzer.getInterpretation().contains("Metabolic Alkalosis"));
}

void BloodGasAnalyzerTest::testHypoxemia()
{
    BloodGasAnalyzer analyzer;
    analyzer.setPH(7.40);
    analyzer.setPCO2(40);
    analyzer.setPO2(65);
    analyzer.setHCO3(24);
    analyzer.setBaseExcess(0);
    
    analyzer.analyze();
    
    QCOMPARE(analyzer.getPO2Status(), QString("Low (Hypoxemia)"));
    QVERIFY(analyzer.getInterpretation().contains("Hypoxemia"));
}

QTEST_MAIN(BloodGasAnalyzerTest)
#include "test_bloodgasanalyzer.moc"