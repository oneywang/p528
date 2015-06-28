#ifndef CHROME_BROWSER_UI_ACCOUNTWIDGET_H
#define CHROME_BROWSER_UI_ACCOUNTWIDGET_H

#include <QTableWidget>

//’Àªß=
class AccountWidget : public QTableWidget{
  Q_OBJECT
public:
  explicit AccountWidget(QWidget *parent = 0);
  ~AccountWidget();

public:
  void InitUi();
};

#endif //CHROME_BROWSER_UI_ACCOUNTWIDGET_H