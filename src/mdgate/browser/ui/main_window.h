#ifndef CHROME_BROWSER_UI_MAINWINDOW_H
#define CHROME_BROWSER_UI_MAINWINDOW_H

#include <QMainWindow>

//Ö÷´°¿Ú=
class MainWindow : public QMainWindow{
  Q_OBJECT
public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

public:
  void InitUi();
};

#endif //CHROME_BROWSER_UI_MAINWINDOW_H