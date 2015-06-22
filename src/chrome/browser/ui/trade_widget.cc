#include "chrome/browser/ui/trade_widget.h"
#include <QHeaderView>

//////////////////////////////////////////////////////////////////////////

TradeWidget::TradeWidget(QWidget* parent /* = 0 */)
  :QTableWidget(parent){
  InitUi();
}

TradeWidget::~TradeWidget(){

}

void TradeWidget::InitUi(){
  QMap<QString, QString> dictLabels;

  dictLabels["0-InstrumentID"] = QStringLiteral("合约代码");
  dictLabels["1-ExchangeInstID"] = QStringLiteral("交易所合约代码");
  dictLabels["2-ExchangeID"] = QStringLiteral("交易所");
  dictLabels["3-Direction"] = QStringLiteral("方向");
  dictLabels["4-OffsetFlag"] = QStringLiteral("开平");
  dictLabels["5-TradeID"] = QStringLiteral("成交编号");
  dictLabels["6-TradeTime"] = QStringLiteral("成交时间");
  dictLabels["7-Volume"] = QStringLiteral("数量");
  dictLabels["8-Price"] = QStringLiteral("价格");
  dictLabels["9-OrderRef"] = QStringLiteral("报单号");
  dictLabels["a-OrderSysID"] = QStringLiteral("报单系统号");

  this->setColumnCount(dictLabels.size());
  this->setHorizontalHeaderLabels(dictLabels.values());
  this->verticalHeader()->setVisible(false);
  this->setEditTriggers(QTableWidget::NoEditTriggers);
}

