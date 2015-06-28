#include "mdgate/browser/ui/order_widget.h"
#include <QHeaderView>

//////////////////////////////////////////////////////////////////////////

OrderWidget::OrderWidget(QWidget* parent /* = 0 */)
  :QTableWidget(parent){
  InitUi();
}

OrderWidget::~OrderWidget(){

}

void OrderWidget::InitUi(){
  QMap<QString, QString> dictLabels;

  dictLabels["0-OrderRef"] = QStringLiteral("报单号");
  dictLabels["1-OrderSysID"] = QStringLiteral("系统编号");
  dictLabels["2-InstrumentID"] = QStringLiteral("合约代码");
  dictLabels["3-ExchangeInstID"] = QStringLiteral("交易所合约代码");
  dictLabels["4-Direction"] = QStringLiteral("方向");
  dictLabels["5-CombOffsetFlag"] = QStringLiteral("开平");
  dictLabels["6-LimitPrice"] = QStringLiteral("价格");
  dictLabels["7-VolumeTotalOriginal"] = QStringLiteral("委托数量");
  dictLabels["8-VolumeTraded"] = QStringLiteral("成交数量");
  dictLabels["9-InsertTime"] = QStringLiteral("委托时间");
  dictLabels["a-CancelTime"] = QStringLiteral("撤销时间");
  dictLabels["b-StatusMsg"] = QStringLiteral("状态信息");

  this->setColumnCount(dictLabels.size());
  this->setHorizontalHeaderLabels(dictLabels.values());
  this->verticalHeader()->setVisible(false);
  this->setEditTriggers(QTableWidget::NoEditTriggers);
}
