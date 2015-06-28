#ifndef CHROME_BROWSER_UI_LOGWIDGET_H
#define CHROME_BROWSER_UI_LOGWIDGET_H

#include <QTableWidget>

//»’÷æ=
class LogWidget : public QTableWidget{
  Q_OBJECT
public:
  explicit LogWidget(QWidget* parent = 0);
  ~LogWidget();

public:
  void InitUi();
};

#endif //CHROME_BROWSER_UI_LOGWIDGET_H