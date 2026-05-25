#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void digit_pressed();
    void decimal_pressed();
    void unary_operation_pressed();
    void clear_pressed();
    void equals_pressed();
    void binary_operation_pressed();
    void expanded_pressed();
    void function_pressed();
    void radDeg_pressed();
    void symbol_pressed();

private:
    Ui::MainWindow *ui;
    QList<QPushButton*> extraButtons;
    QList<QPushButton*> symbolButtons;
    QList<QPushButton*> hiddenButtons;
    bool check_parentheses(const std::string& expression);
    void clearExpression();
};
#endif // MAINWINDOW_H
