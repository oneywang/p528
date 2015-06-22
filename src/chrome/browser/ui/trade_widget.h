#ifndef CHROME_BROWSER_UI_TRADE_WIDGET_H
#define CHROME_BROWSER_UI_TRADE_WIDGET_H

#include <QTableWidget>

//³É½»=
class TradeWidget : public QTableWidget{
  Q_OBJECT
public:
  explicit TradeWidget(QWidget* parent = 0);
  ~TradeWidget();

public:
  void InitUi();
};

#endif //CHROME_BROWSER_UI_TRADE_WIDGET_H