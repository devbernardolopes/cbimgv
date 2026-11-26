#include "mainwindow.h"

#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  setWindowTitle("cbimgv");
  resize(800, 600);

  auto *label = new QLabel("Hello, Qt 6!");

  label->setAlignment(Qt::AlignCenter);

  setCentralWidget(label);
}