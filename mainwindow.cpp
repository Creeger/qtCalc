#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <shuntingYard.h>
#include "compute.h"
#include <QtDebug>
#include <stack>
#include <QLayout>
#include <utility>
#include <unordered_map>

#include <iostream>

// TODO: handle math consts and make tests for them

bool equalsPressed = false;
bool operatorPressed = false;
int hiddenWidth = 241, hiddenHeight = 312; // Hopefully a temporary solution
int visibleWidth = 301, visibleheight = 414; // Ideally these sizes would be retrieved and adjusted automatically


AngleMode toAngleMode(const QString& s);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << "Constructor running";

    QObject::connect(ui->pushButton_clear, &QPushButton::clicked, this, &MainWindow::clear_pressed);

    // Connect every digit button to the ui window
    for (int i = 0; i <= 9; ++i) {
        QString name = QString("pushButton_%1").arg(i);
        QPushButton *button = findChild<QPushButton *>(name);

        if (button) {
            connect(button, &QPushButton::clicked, this, &MainWindow::digit_pressed);
        }
    }

    QObject::connect(ui->pushButton_percent, &QPushButton::clicked, this, &MainWindow::unary_operation_pressed);
    QObject::connect(ui->pushButton_plusminus, &QPushButton::clicked, this, &MainWindow::unary_operation_pressed);
    QObject::connect(ui->pushButton_decimal, &QPushButton::clicked, this, &MainWindow::decimal_pressed);

    QObject::connect(ui->pushButton_add, &QPushButton::clicked, this, &MainWindow::binary_operation_pressed);
    QObject::connect(ui->pushButton_subtract, &QPushButton::clicked, this, &MainWindow::binary_operation_pressed);
    QObject::connect(ui->pushButton_divide, &QPushButton::clicked, this, &MainWindow::binary_operation_pressed);
    QObject::connect(ui->pushButton_multiply, &QPushButton::clicked, this, &MainWindow::binary_operation_pressed);

    QObject::connect(ui->pushButton_equals, &QPushButton::clicked, this, &MainWindow::equals_pressed);
    QObject::connect(ui->pushButton_expand, &QPushButton::clicked, this, &MainWindow::expanded_pressed);


    QObject::connect(ui->pushButton_radDeg, &QPushButton::clicked, this, &MainWindow::radDeg_pressed);
    ui->pushButton_radDeg->setVisible(false);


    extraButtons = {
        ui->pushButton_sin,
        ui->pushButton_cos,
        ui->pushButton_tan,
        ui->pushButton_ln,
        ui->pushButton_log,
        ui->pushButton_max,
        ui->pushButton_power,
        ui->pushButton_sqrt,
    };

    symbolButtons = {
        ui->pushButton_opening,
        ui->pushButton_closing,
        ui->pushButton_pi,
        ui->pushButton_epsilon,
        ui->pushButton_tau,
    };

    hiddenButtons << extraButtons << symbolButtons << ui->pushButton_radDeg;

    for (QPushButton *btn : extraButtons) {
        QObject::connect(btn, &QPushButton::clicked, this, &MainWindow::function_pressed);
    }

    for (QPushButton *btn : symbolButtons) {
        QObject::connect(btn, &QPushButton::clicked, this, &MainWindow::symbol_pressed);
    }

    for (QPushButton *btn : hiddenButtons) {
        btn->setVisible(false);
    }

    static const std::unordered_map<std::string, double> mathConsts = {
        {"π", 3.141592653589793},
        {"ε", 2.718281828459045},
        {"τ", 6.283185307179586},
    };

    ui->label->setText("0");
    ui->expressionLabel->setText("");

    // width, height
    resize(hiddenWidth, hiddenHeight);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::digit_pressed() {
    if (equalsPressed) {
        clear_pressed();
    }

    QPushButton *button = (QPushButton*)sender();
    double labelNumber;
    QString newLabel;

    labelNumber = button->text().toDouble();
    if (operatorPressed) {
        labelNumber = (button->text()).toDouble();
    } else {
        labelNumber = (ui->label->text() + button->text()).toDouble();
    }

    newLabel = QString::number(labelNumber, 'g', 15);
    ui->label->setText(newLabel);
    ui->expressionLabel->setText((ui->expressionLabel->text() + button->text()));
}

void MainWindow::decimal_pressed() {
    if (equalsPressed) {
        clear_pressed();
    }

    QString expression = ui->expressionLabel->text();
    QChar lastChar;

    if (expression.length() > 0) {
        lastChar = expression[expression.length() - 1];
    }

    if (!expression.isEmpty() && !lastChar.isDigit()) {
        ui->label->setText("Error");
    } else {
        ui->label->setText(ui->label->text() + ",");
        ui->expressionLabel->setText(ui->expressionLabel->text() + ",");
    }
}

void MainWindow::unary_operation_pressed() {
    if (equalsPressed) {
        clear_pressed();
    }

    QPushButton *btn = (QPushButton*)sender();
    double labelNumber;
    QString newLabel;
    labelNumber = ui->label->text().toDouble();

    if (btn->text() == "+/-" && labelNumber != 0) {
        labelNumber = labelNumber * -1;
        newLabel = QString::number(labelNumber, 'g', 15);
        ui->label->setText(newLabel);
        ui->expressionLabel->setText(ui->expressionLabel->text() + newLabel);
    }

    if (btn->text() == "%") {
        labelNumber = labelNumber * 0.01;
        newLabel = QString::number(labelNumber, 'g', 15);
        ui->label->setText(newLabel);
    }
    operatorPressed = true;
}

void MainWindow::clear_pressed() {
    ui->label->setText("0");
    ui->expressionLabel->clear();
    equalsPressed = false;
}

void MainWindow::binary_operation_pressed() {
    if (equalsPressed) {
        clear_pressed();
    }

    QPushButton *btn = (QPushButton*)sender();
    ui->expressionLabel->setText(ui->expressionLabel->text() + btn->text());
    operatorPressed = true;
}

void MainWindow::equals_pressed() {
    QString expressionDisplay = ui->expressionLabel->text();

    if (expressionDisplay.endsWith("=")) {
        return;
    }

    std::string expression = ui->expressionLabel->text().toStdString();

    if (!check_parentheses(expression)) {
        return;
    }

    AngleMode mode = toAngleMode(ui->pushButton_radDeg->text());
    std::vector<std::string> tokens = tokenize(expression);
    std::vector<std::string> rpn = shuntingYard(tokens);
    EvalResult result = computeRPN(rpn, mode);

    if (!result.valid) {
        ui->label->setText(QString("Invalid result: %1\n").arg(QString::fromStdString(result.error)));
        return;
    }

    ui->expressionLabel->setText(expressionDisplay + "=");
    ui->label->setText(QString::number(result.result));

    equalsPressed = true;
}

void MainWindow::expanded_pressed() {
    bool visible = hiddenButtons.first()->isVisible();


    for (auto *btn : MainWindow::hiddenButtons) {
        btn->setVisible(!visible);
    }

    ui->centralwidget->adjustSize();

    if (!visible) {
        resize(visibleWidth, visibleheight);
    } else {
        resize(hiddenWidth, hiddenHeight);
    }
}

void MainWindow::function_pressed() {
    if (equalsPressed) {
        clearExpression();
    }
    QPushButton *btn = (QPushButton*)sender();
    ui->expressionLabel->setText(ui->expressionLabel->text() + btn->text());
}

void MainWindow::radDeg_pressed() {
    QPushButton *btn = (QPushButton*)sender();
    QString txt = ui->pushButton_radDeg->text();

    if (txt == "RAD") {
        ui->pushButton_radDeg->setText("DEG");
    } else {
        ui->pushButton_radDeg->setText("RAD");
    }
}

void MainWindow::symbol_pressed() {
    QPushButton *btn = (QPushButton*)sender();

    ui->expressionLabel->setText(ui->expressionLabel->text() + btn->text());
}


bool MainWindow::check_parentheses(const std::string& expression) {
    std::stack<char> st;

    if (expression.empty()) return false;
    for (char c : expression) {
        if (c == '(' || c == '{' || c == '[') {
            st.push(c);
        } else if (c == ')' || c == '}' || c == ']') {
            if (st.empty()) return false;
            char tp = st.top();
            if ((c == ')' && tp != '(') ||
                (c == '}' && tp != '{') ||
                (c == ']' && tp != '[')) {
                return false;
            }
            st.pop();
        }
    }
    return st.empty();
}

AngleMode toAngleMode(const QString& s) {
    if (s.compare("DEG", Qt::CaseInsensitive) == 0) {
        return AngleMode::DEG;
    }
    return AngleMode::RAD; // default fallback
}

void MainWindow::clearExpression() {
    ui->label->clear();
    ui->expressionLabel->clear();
    equalsPressed = false;
}
