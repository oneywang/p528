#ifndef CHROME_BROWSER_UI_ORDER_WIDGET_H
#define CHROME_BROWSER_UI_ORDER_WIDGET_H

#include <QTableWidget>

//±¨µ¥=
class OrderWidget : public QTableWidget{
  Q_OBJECT
public:
  explicit OrderWidget(QWidget* parent = 0);
  ~OrderWidget();

public:
  void InitUi();
};


#endif //CHROME_BROWSER_UI_ORDER_WIDGET_H