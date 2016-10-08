#include "AbstractSearchModel.h"

AbstractSearchModelInterface::AbstractSearchModelInterface(){}
AbstractSearchModelInterface::~AbstractSearchModelInterface(){}

AbstractSearchTableModel::AbstractSearchTableModel(QObject* parent) :
    QAbstractTableModel(parent),
    AbstractSearchModelInterface()
{}

AbstractSearchTableModel::~AbstractSearchTableModel(){}

AbstractSearchListModel::AbstractSearchListModel(QObject* parent) :
    QAbstractListModel(parent),
    AbstractSearchModelInterface()
{}

AbstractSearchListModel::~AbstractSearchListModel(){}
