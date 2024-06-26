#ifndef CREDITWINDOW_H
#define CREDITWINDOW_H

#include <QDialog>
#include <QRegularExpressionValidator>

#include "../controller/controller.h"

namespace Ui {
class CreditWindow;
}

namespace s21 {

class CreditWindow : public QDialog {
  Q_OBJECT

 public:
  explicit CreditWindow(QWidget *parent = nullptr, Controller *c = nullptr);
  ~CreditWindow();
  bool GetType();
  double GetPeriod();
  double GetCreditAmount();
  double GetRate();

 private slots:
  void on_calculate_clicked();

 private:
  Ui::CreditWindow *ui;
  Controller *controller_credit_;
};
}  // namespace s21

#endif  // CREDITWINDOW_H
