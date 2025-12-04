#ifndef STYLEHELPER_H
#define STYLEHELPER_H

#include <QString>

class StyleHelper {
public:
    static QString getStyle() {
        return R"(
            /* --- GLOBAL WIDGET STYLE --- */
            QWidget {
                background-color: #2b2b2b;
                color: #ffffff;
                font-family: 'Segoe UI', sans-serif;
                font-size: 14px;
            }

            /* --- LABELS --- */
            QLabel {
                color: #e0e0e0;
                font-weight: bold;
            }

            /* --- LINE EDITS --- */
            QLineEdit {
                background-color: #3a3a3a;
                border: 2px solid #555555;
                border-radius: 8px;
                padding: 8px;
                color: #ffffff;
                selection-background-color: #3498db;
            }
            QLineEdit:focus {
                border: 2px solid #3498db;
            }

            /* --- BUTTONS --- */
            QPushButton {
                background-color: #3498db;
                color: white;
                border: none;
                border-radius: 8px;
                padding: 10px 20px;
                font-weight: bold;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #2980b9;
            }
            QPushButton:pressed {
                background-color: #1f618d;
            }
            QPushButton:disabled {
                background-color: #555555;
                color: #aaaaaa;
            }

            /* --- FLAT BUTTONS (Like 'Signup' link) --- */
            QPushButton[flat="true"] {
                background-color: transparent;
                color: #3498db;
                text-decoration: underline;
                border: none;
            }
            QPushButton[flat="true"]:hover {
                color: #5dade2;
            }

            /* --- RADIO BUTTONS --- */
            QRadioButton {
                color: #e0e0e0;
                spacing: 5px;
            }
            QRadioButton::indicator {
                width: 18px;
                height: 18px;
            }
            QRadioButton::indicator:unchecked {
                border: 2px solid #555555;
                border-radius: 10px;
                background-color: #3a3a3a;
            }
            QRadioButton::indicator:checked {
                border: 2px solid #3498db;
                border-radius: 10px;
                background-color: #3498db;
            }

            /* --- MESSAGE BOX --- */
            QMessageBox {
                background-color: #2b2b2b;
            }
            QMessageBox QLabel {
                color: #ffffff;
            }
            QMessageBox QPushButton {
                width: 80px;
            }
        )";
    }
};

#endif // STYLEHELPER_H
