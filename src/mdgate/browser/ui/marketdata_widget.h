#ifndef CHROME_BROWSER_UI_MARKDATAWIDGET_H
#define CHROME_BROWSER_UI_MARKDATAWIDGET_H

#include <QTableWidget>

//ÐÐÇé=
class MarketDataWidget : public QTableWidget{
  Q_OBJECT
public:
  explicit MarketDataWidget(QWidget* parent = 0);
  ~MarketDataWidget();

public:
  void InitUi();
};

#endif //CHROME_BROWSER_UI_MARKDATAWIDGET_H