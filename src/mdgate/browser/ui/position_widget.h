#ifndef CHROME_BROWSER_UI_POSITION_WIDGET_H
#define CHROME_BROWSER_UI_POSITION_WIDGET_H

#include <QTableWidget>

//³Ö²Ö=
class PositionWidget : public QTableWidget{
  Q_OBJECT
public:
  explicit PositionWidget(QWidget* parent = 0);
  ~PositionWidget();

public:
  void InitUi();
};

#endif //CHROME_BROWSER_UI_POSITION_WIDGET_H