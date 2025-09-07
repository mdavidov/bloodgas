#include "bloodgasanalyzer.h"
#include <cmath>

BloodGasAnalyzer::BloodGasAnalyzer()
    : m_pH(0.0), m_pCO2(0.0), m_pO2(0.0), m_hco3(0.0), m_baseExcess(0.0)
{
}

void BloodGasAnalyzer::setPH(double pH)
{
    m_pH = pH;
}

void BloodGasAnalyzer::setPCO2(double pCO2)
{
    m_pCO2 = pCO2;
}

void BloodGasAnalyzer::setPO2(double pO2)
{
    m_pO2 = pO2;
}

void BloodGasAnalyzer::setHCO3(double hco3)
{
    m_hco3 = hco3;
}

void BloodGasAnalyzer::setBaseExcess(double baseExcess)
{
    m_baseExcess = baseExcess;
}

void BloodGasAnalyzer::analyze()
{
    QString primary, secondary, oxygenation;
    
    // Primary disorder analysis
    if (isAcidemic()) {
        if (isRespiratoryAcidosis()) {
            primary = "Respiratory Acidosis";
        } else if (isMetabolicAcidosis()) {
            primary = "Metabolic Acidosis";
        } else {
            primary = "Mixed Acidosis";
        }
    } else if (isAlkalemic()) {
        if (isRespiratoryAlkalosis()) {
            primary = "Respiratory Alkalosis";
        } else if (isMetabolicAlkalosis()) {
            primary = "Metabolic Alkalosis";
        } else {
            primary = "Mixed Alkalosis";
        }
    } else {
        primary = "Normal pH";
    }
    
    // Compensation analysis
    if (isCompensated()) {
        secondary = " (Fully Compensated)";
    } else if (isPartiallyCompensated()) {
        secondary = " (Partially Compensated)";
    } else {
        secondary = " (Uncompensated)";
    }
    
    // Oxygenation analysis
    if (isHypoxemic()) {
        oxygenation = "\nHypoxemia present (pO2 < 80 mmHg)";
    } else {
        oxygenation = "\nOxygenation adequate";
    }
    
    m_interpretation = primary + secondary + oxygenation;
}

QString BloodGasAnalyzer::getInterpretation() const
{
    return m_interpretation;
}

QString BloodGasAnalyzer::getPHStatus() const
{
    if (m_pH < 7.35) return "Low (Acidic)";
    if (m_pH > 7.45) return "High (Alkaline)";
    return "Normal";
}

QString BloodGasAnalyzer::getPCO2Status() const
{
    if (m_pCO2 < 35) return "Low";
    if (m_pCO2 > 45) return "High";
    return "Normal";
}

QString BloodGasAnalyzer::getPO2Status() const
{
    if (m_pO2 < 80) return "Low (Hypoxemia)";
    if (m_pO2 > 100) return "High";
    return "Normal";
}

QString BloodGasAnalyzer::getHCO3Status() const
{
    if (m_hco3 < 22) return "Low";
    if (m_hco3 > 28) return "High";
    return "Normal";
}

QString BloodGasAnalyzer::getBaseExcessStatus() const
{
    if (m_baseExcess < -2) return "Negative (Base Deficit)";
    if (m_baseExcess > 2) return "Positive (Base Excess)";
    return "Normal";
}

bool BloodGasAnalyzer::isAcidemic() const
{
    return m_pH < 7.35;
}

bool BloodGasAnalyzer::isAlkalemic() const
{
    return m_pH > 7.45;
}

bool BloodGasAnalyzer::isRespiratoryAcidosis() const
{
    return m_pCO2 > 45;
}

bool BloodGasAnalyzer::isRespiratoryAlkalosis() const
{
    return m_pCO2 < 35;
}

bool BloodGasAnalyzer::isMetabolicAcidosis() const
{
    return m_hco3 < 22 || m_baseExcess < -2;
}

bool BloodGasAnalyzer::isMetabolicAlkalosis() const
{
    return m_hco3 > 28 || m_baseExcess > 2;
}

bool BloodGasAnalyzer::isCompensated() const
{
    // Simplified compensation check
    if (isAcidemic()) {
        return (isRespiratoryAcidosis() && m_hco3 > 28) || 
               (isMetabolicAcidosis() && m_pCO2 < 35);
    } else if (isAlkalemic()) {
        return (isRespiratoryAlkalosis() && m_hco3 < 22) || 
               (isMetabolicAlkalosis() && m_pCO2 > 45);
    }
    return false;
}

bool BloodGasAnalyzer::isPartiallyCompensated() const
{
    // Simplified partial compensation check
    if (std::abs(m_pH - 7.4) > 0.05) {
        if (isAcidemic()) {
            return (isRespiratoryAcidosis() && m_hco3 > 24) || 
                   (isMetabolicAcidosis() && m_pCO2 < 40);
        } else if (isAlkalemic()) {
            return (isRespiratoryAlkalosis() && m_hco3 < 24) || 
                   (isMetabolicAlkalosis() && m_pCO2 > 40);
        }
    }
    return false;
}

bool BloodGasAnalyzer::isHypoxemic() const
{
    return m_pO2 < 80;
}
