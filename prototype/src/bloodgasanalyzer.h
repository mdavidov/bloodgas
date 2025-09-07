#pragma once

#include <QString>

class BloodGasAnalyzer
{
public:
    BloodGasAnalyzer();
    
    // Setters
    void setPH(double pH);
    void setPCO2(double pCO2);
    void setPO2(double pO2);
    void setHCO3(double hco3);
    void setBaseExcess(double baseExcess);
    
    // Getters
    double getPH() const { return m_pH; }
    double getPCO2() const { return m_pCO2; }
    double getPO2() const { return m_pO2; }
    double getHCO3() const { return m_hco3; }
    double getBaseExcess() const { return m_baseExcess; }
    
    // Analysis methods
    void analyze();
    QString getInterpretation() const;
    
    // Status methods
    QString getPHStatus() const;
    QString getPCO2Status() const;
    QString getPO2Status() const;
    QString getHCO3Status() const;
    QString getBaseExcessStatus() const;

private:
    double m_pH;
    double m_pCO2;
    double m_pO2;
    double m_hco3;
    double m_baseExcess;
    
    QString m_interpretation;
    
    // Analysis helpers
    bool isAcidemic() const;
    bool isAlkalemic() const;
    bool isRespiratoryAcidosis() const;
    bool isRespiratoryAlkalosis() const;
    bool isMetabolicAcidosis() const;
    bool isMetabolicAlkalosis() const;
    bool isCompensated() const;
    bool isPartiallyCompensated() const;
    bool isHypoxemic() const;
};
