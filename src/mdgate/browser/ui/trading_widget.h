#ifndef CHROME_BROWSER_UI_TRADING_WIDGET_H
#define CHROME_BROWSER_UI_TRADING_WIDGET_H

#include <QWidget>
class QPushButton;
class MdApi;

//½»Ò×=
class TradingWidget : public QWidget{
  Q_OBJECT
public:
  explicit TradingWidget(QWidget* parent = 0);
  ~TradingWidget();

public:
  void InitUi();
  void RegisterEvent();

private slots:
  void Setup();
  void Connect();
  void Login();
  void Logout();
  void Reset();

private:
  MdApi* mdapi_ = nullptr;

private:
  QPushButton* buy_button;
  QPushButton* cancel_button;
  QPushButton* login_button;
  QPushButton* logoff_button;
  QPushButton* about_button;
  QPushButton* close_button;
  QPushButton* init_button;
  QPushButton* connect_button;
  QPushButton* fini_button;
};

#endif //CHROME_BROWSER_UI_TRADING_WIDGET_H
