# Blood Gas Analyzer

A comprehensive C++ Qt 6 application with QML UI for blood gas analysis in medical environments.

## Features

### Core Functionality

- **Touch-friendly QML UI** designed for medical device use
- **Blood gas analysis simulation** with realistic parameter ranges (pH, pO₂, pCO₂, HCO₃, SO₂, etc.)
- **Secure user authentication** with role-based permissions (Administrator, Supervisor, Operator)
- **Session management** with automatic timeout and session extension
- **SQLite database** with encryption support for data persistence

### Data Management

- **HistoricalDataModel** (QAbstractListModel) for efficient data handling
- **ListView in QML** for displaying historical results with filtering
- **Export functionality** for CSV data export
- **Comprehensive audit trail** for regulatory compliance

### Device Integration

- **Calibration workflow** with animated feedback and step-by-step guidance
- **HL7 integration** for hospital information systems
- **Quality control** and validation procedures
- **Equipment status monitoring**

### User Interface

- **Multi-view navigation** with responsive design
- **Real-time status indicators** and progress feedback
- **Touch-optimized controls** with haptic-like feedback
- **Professional medical device styling**

## Technical Architecture

### C++ Backend Components

- `BloodGasAnalyzer` - Main application controller
- `HistoricalDataModel` - QAbstractListModel for data management
- `DatabaseManager` - SQLite database with encryption
- `AuthenticationManager` - User login and session management
- `CalibrationManager` - Device calibration workflow
- `HL7Manager` - Hospital system integration

### QML Frontend Views

- `main.qml` - Main application window with navigation
- `LoginView.qml` - Secure user authentication
- `MainView.qml` - Dashboard and quick actions
- `SampleInputView.qml` - Touch-friendly sample input and results
- `CalibrationView.qml` - Calibration workflow with animations
- `HistoryView.qml` - Historical data with filtering and export

### Reusable Components

- `TouchButton.qml` - Touch-optimized button with animations
- `InputField.qml` - Medical-grade input field with validation

### Default Login Credentials

- **Administrator**: `admin` / `admin123`
- **Operator**: `operator` / `operator123`

## Usage

### Initial Setup

1. Launch the application
2. Log in with the provided credentials
3. Perform initial device calibration
4. Begin sample analysis

### Sample Analysis Workflow

1. Navigate to "New Analysis" from the main dashboard
2. Enter sample information (Sample ID, Patient ID, Temperature)
3. Select analysis parameters (Blood Gas, Electrolytes, Metabolites)
4. Start analysis and wait for results
5. Review results with normal range indicators
6. Export or send results via HL7

### Calibration Procedure

1. Access the "Calibration" view
2. Select calibration type (Full, pH Only, Gas Sensors, Electrolytes)
3. Follow the step-by-step guided procedure
4. Monitor progress with animated feedback
5. Review calibration completion status

### Historical Data

1. View historical results in the "History" view
2. Filter by patient ID, operator, or date range
3. Export data to CSV format
4. Maintain audit trail for compliance

## Database Schema

The application uses SQLite with the following main tables:

- `users` - User authentication and roles
- `results` - Blood gas analysis results
- `calibrations` - Calibration history and data
- `audit_log` - Complete audit trail

## Security Features

- **Encrypted password storage** using salted SHA-256 hashes
- **Session timeout** with configurable duration
- **Role-based access control** for different user types
- **Comprehensive audit logging** for regulatory compliance
- **Data encryption** for sensitive medical information

## Compliance & Standards

The application is designed with medical device standards in mind:

- IVD (In Vitro Diagnostic) device considerations
- FDA 21 CFR Part 820 quality system compliance framework
- HIPAA privacy considerations for patient data
- HL7 messaging standards for interoperability

## Architecture Highlights

- **Qt 6 with QML** for modern, touch-friendly UI
- **C++ backend** for performance-critical operations
- **SQLite database** for reliable data persistence
- **Modular design** for maintainability and testing
- **Cross-platform compatibility** (Windows, Linux, macOS)

## Future Enhancements

- Integration with laboratory information systems (LIS)
- Real-time data transmission via TCP/IP
- Advanced statistical analysis and trending
- Barcode scanner integration
- Printer support for result reports
- Multi-language localization

### Gas Value Ranges

- **pH Status**: Normal (7.35-7.45), Acidic (<7.35), Alkaline (>7.45)
- **pCO2 Status**: Normal (35-45 mmHg), Low (<35), High (>45)
- **pO2 Status**: Normal (>80 mmHg), Hypoxemia (<80)
- **HCO3- Status**: Normal (22-28 mEq/L), Low (<22), High (>28)
- **Base Excess Status**: Normal (-2 to +2 mEq/L), Deficit (<-2), Excess (>2)
- **Interpretation**: Respiratory/Metabolic Acidosis/Alkalosis with compensation status

### Error Handling

- Input validation for all blood gas values
- Range checking (pH: 6.8-8.0, other values must be positive)
- Graceful error messages for invalid inputs
- Comprehensive unit test coverage for edge cases

## NOTE

**This is a demonstration application for educational purposes. For production use in medical environments, additional validation, testing, and regulatory approval would be required.**

## Supported platforms

- Windows
- Mac
- Linux

## Install app binaries

Visit https://github.com/mdavidov/bloodgasanalyzer/releases/,
click the latest release name and select the installer for your platform.

- Windows: installer-windows-intel-x86_64.exe
- Mac: installer-mac-apple-arm64.app.zip
- Linux: TODO (can build from source in the meantime, see below)

## Uninstall

This app can be uninstalled by running the maintenancetool.exe / maintenancetool.app.
It's located in the same folder where foldersearch.exe / foldersearch.app is installed;
by default it's $HOME/DevOnline/foldersearch.

## Build from source

### Prerequisites

Ensure the following dependencies are installed:

- Qt 6
- CMake (>= 3.20)
- Ninja
- Optionally install VS Code (highly recommended).

C++ compiler:

- Windows: MSVC is recommended (or MinGW).
- Linux: GCC 14 (g++-14) or Clang
- macOS: Xcode (Clang) or GCC 14

### Get source

```bash
git clone https://github.com/mdavidov/bloodgasanalyzer
cd bloodgasanalyzer
```

### Build

On all three supported platforms VS Code can be used, but CMAKE_PREFIX_PATH
must be first set in the CMakeLists.txt file (above the line that checks
if CMAKE_PREFIX_PATH is not defined):

```cmake
set(CMAKE_PREFIX_PATH "/path/to/Qt")
```

Replace /path/to/Qt with the path to your Qt installation
(e.g. C:/Qt/6.9.1/msvc2022_64 or /opt/Qt/6.9.1/macos)

- Run VS Code and open the cloned sources folder (e.g. ```code .``` or ```code bloodgasanalyzer``` if you didn't cd into it)
- If the compiler-kit-selection drop-down opens, then select your preferred one
- VS Code bootstraps the build automatically
- Press F7 to build

Otherwise follow the below instructions.

#### Windows

It's best to have Visual Studio (e.g. 2022) installed.
Open "Developer PowerShell for VS 2022" or "Developer Command Prompt for VS 2022".
__NOTE__: Visual Studio Community Edition is free (for non-commercial use).

Assuming you changed dir to bloodgasanalyzer do this:

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH=/path/to/Qt
cmake --build . --config Debug  # or Release
```

Replace /path/to/Qt with the path to your Qt installation
(e.g. C:/Qt/6.9.1/msvc2022_64 or /opt/Qt/6.9.1/macos)

On Windows instead of building from the command line,
you can use Visual Studio to open the CMake generated
solution file (foldersearch.sln). Then build the app from
menu BUILD -> Build Solution or press F7.

#### Mac and Linux

Assuming you changed directory to bloodgasanalyzer do this:

```bash
mkdir build && cd build
cmake .. -G "Ninja Multi-Config" -DCMAKE_PREFIX_PATH=/path/to/Qt
ninja
# or
cmake --build . --config Debug  # or Release
```

Replace /path/to/Qt with the path to your Qt installation
(e.g. C:/Qt/6.9.1/msvc2022_64 or /opt/Qt/6.9.1/macos)
