#include "mdgate/browser/ui/account_widget.h"

#include <QMap>
#include <QHeaderView>

//////////////////////////////////////////////////////////////////////////

AccountWidget::AccountWidget(QWidget *parent /* = 0 */)
  :QTableWidget(parent){

  InitUi();
}

AccountWidget::~AccountWidget(){

}

//////////////////////////////////////////////////////////////////////////

void AccountWidget::InitUi(){
  QMap<QString,QString> dictLabels;
  
  dictLabels["0-AccountID"] = QStringLiteral("投资者账户");
  dictLabels["1-PreBalance"] = QStringLiteral("昨结");
  dictLabels["2-Withdraw"] = QStringLiteral("出金");
  dictLabels["3-Deposit"] = QStringLiteral("入金");
  dictLabels["4-FrozenCash"] = QStringLiteral("冻结资金");
  dictLabels["5-FrozenMargin"] = QStringLiteral("冻结保证金");
  dictLabels["6-FrozenCommission"] = QStringLiteral("冻结手续费");
  dictLabels["7-Commission"] = QStringLiteral("手续费");
  dictLabels["8-CurrMargin"] = QStringLiteral("当前保证金");
  dictLabels["9-Available"] = QStringLiteral("可用资金");
  dictLabels["a-WithdrawQuota"] = QStringLiteral("可取资金");
  dictLabels["b-Balance"] = QStringLiteral("账户资金");
  
  this->setColumnCount(dictLabels.size());
  this->setHorizontalHeaderLabels(dictLabels.values());
  this->verticalHeader()->setVisible(false);
  this->setEditTriggers(QTableWidget::NoEditTriggers);
}

//////////////////////////////////////////////////////////////////////////