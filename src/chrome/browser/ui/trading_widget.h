#ifndef CHROME_BROWSER_UI_TRADING_WIDGET_H
#define CHROME_BROWSER_UI_TRADING_WIDGET_H

#include <QWidget>

//½»Ò×=
class TradingWidget : public QWidget{
  Q_OBJECT
public:
  explicit TradingWidget(QWidget* parent = 0);
  ~TradingWidget();

public:
  void InitUi();
};

#endif //CHROME_BROWSER_UI_TRADING_WIDGET_H
