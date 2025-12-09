#ifndef STYLES_H
#define STYLES_H

#include <QString>

class Styles {
public:
    static QString getGlobalStyleSheet() {
        return R"(
/* PALETTE DE COULEURS */
QMainWindow, QDialog, QWidget#centralwidget, QWidget#pageProf, QWidget#pageStudent {
    background-color: #F8F9FA;
}

QWidget {
    font-family: "Segoe UI", "Roboto", Arial, sans-serif;
    color: #202124;
}

/* LABELS */
QLabel {
    color: #202124;
    background: transparent;
}

QLabel#labelTitle {
    color: #4285F4;
    font-size: 28px;
    font-weight: 600;
    padding: 20px;
}

QLabel#lblWelcomeProf, QLabel#lblWelcomeStudent {
    color: #202124;
    font-size: 24px;
    font-weight: 500;
    padding: 15px;
}

/* INPUTS */
QLineEdit {
    background-color: white;
    border: 2px solid #E0E0E0;
    border-radius: 8px;
    padding: 12px 16px;
    font-size: 14px;
    color: #202124;
    min-height: 20px;
}

QLineEdit:focus {
    border: 2px solid #4285F4;
    background-color: #FFFFFF;
}

QLineEdit:hover {
    border-color: #BDBDBD;
}

/* TEXT EDIT */
QTextEdit {
    background-color: white;
    border: 2px solid #E0E0E0;
    border-radius: 8px;
    padding: 12px;
    font-size: 14px;
    color: #202124;
}

QTextEdit:focus {
    border: 2px solid #4285F4;
}

/* COMBOBOX */
QComboBox {
    background-color: white;
    border: 2px solid #E0E0E0;
    border-radius: 8px;
    padding: 10px 16px;
    font-size: 14px;
    color: #202124;
    min-height: 20px;
}

QComboBox:hover {
    border-color: #4285F4;
}

QComboBox:focus {
    border: 2px solid #4285F4;
}

QComboBox QAbstractItemView {
    background-color: white;
    border: 1px solid #E0E0E0;
    border-radius: 8px;
    selection-background-color: #E8F0FE;
    selection-color: #202124;
    padding: 5px;
}

/* RADIO BUTTONS */
QRadioButton {
    color: #202124;
    background: transparent;
    spacing: 8px;
    font-size: 14px;
}

QRadioButton::indicator {
    width: 20px;
    height: 20px;
    border-radius: 10px;
    border: 2px solid #5F6368;
    background-color: white;
}

QRadioButton::indicator:checked {
    background-color: #4285F4;
    border: 2px solid #4285F4;
}

QRadioButton::indicator:hover {
    border-color: #4285F4;
}

/* BOUTONS */
QPushButton {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #4285F4, stop:1 #1a73e8);
    color: white;
    border: none;
    border-radius: 8px;
    padding: 12px 24px;
    font-size: 14px;
    font-weight: 600; /* Plus gras */
    min-height: 20px;
}

QPushButton:hover {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #5294FF, stop:1 #2D80EB);
}

QPushButton:pressed {
    background-color: #2952B3;
}

QPushButton:disabled {
    background-color: #E0E0E0;
    color: #9E9E9E;
}

/* Bouton flat */
QPushButton[flat="true"] {
    background-color: transparent;
    color: #4285F4;
    text-decoration: underline;
}

QPushButton[flat="true"]:hover {
    background-color: #E8F0FE;
    text-decoration: none;
}

/* Boutons Déconnexion */
QPushButton#btnLogoutProf, QPushButton#btnLogoutStudent {
    background-color: transparent;
    color: #5F6368;
    border: 2px solid #DADCE0;
    padding: 8px 16px;
}

QPushButton#btnLogoutProf:hover, QPushButton#btnLogoutStudent:hover {
    background-color: #F8F9FA;
    border-color: #4285F4;
    color: #4285F4;
}

/* GROUPBOX (Cards) */
QGroupBox {
    background-color: white;
    border: 1px solid #DADCE0; /* Plus subtil */
    border-radius: 12px;
    margin-top: 1.5em; /* Un peu plus d'espace pour le titre */
    padding: 20px;
    font-size: 14px;
    /* Ombre légère simulée (border-bottom plus épais) */
    border-bottom: 2px solid #D2D4D7; 
}

QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 10px;
    padding: 0 5px;
    color: #1a73e8; /* Bleu Google pour le titre */
    font-weight: bold;
    font-size: 15px;
    background-color: transparent;
}

/* LIST WIDGET */
QListWidget {
    background-color: transparent;
    border: none;
    outline: none;
}

QListWidget::item {
    background-color: white;
    border: 1px solid #E0E0E0;
    border-radius: 12px;
    padding: 16px;
    margin-bottom: 12px;
}

QListWidget::item:hover {
    background-color: #F8F9FA;
    border-color: #4285F4;
}

QListWidget::item:selected {
    background-color: #E8F0FE;
    border-color: #4285F4;
}

/* SCROLLBAR */
QScrollBar:vertical {
    background-color: #F8F9FA;
    width: 12px;
    border-radius: 6px;
}

QScrollBar::handle:vertical {
    background-color: #DADCE0;
    border-radius: 6px;
    min-height: 30px;
}

QScrollBar::handle:vertical:hover {
    background-color: #BDC1C6;
}

QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    height: 0px;
}

/* PAGE LOGIN avec gradient */
QWidget#pageLogin {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #4285F4, stop:1 #34A853);
}

QWidget#pageLogin QLabel#labelTitle {
    color: white;
}

QWidget#pageLogin QLabel {
    color: white;
}
        )";
    }
};

#endif // STYLES_H