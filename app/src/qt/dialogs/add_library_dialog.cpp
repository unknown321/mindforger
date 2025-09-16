/*
 add_library_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "add_library_dialog.h"

using namespace std;

namespace m8r {

AddLibraryDialog::AddLibraryDialog(QWidget* parent)
    : QDialog(parent)
{
    // widgets
    findLibrarySourceLabel = new QLabel{
        tr(
            "Choose a directory (library) of PDF files to be indexed. MindForger\n"
            "will create new notebook for every library file. Such notebook can be\n"
            "used to easily open the library file and create library file related\n"
            "notes.\n\n"
            "Choose new library source:"),
        parent};
    findDirectoryButton = new QPushButton{tr("Directory")};

    uriLabel = new QLabel{tr("Library source path:"), parent};
    uriEdit = new QLineEdit{parent};

    pdfCheckBox = new QCheckBox{tr("index PDF")};
    pdfCheckBox->setChecked(true);
    pdfCheckBox->setEnabled(true);

    // IMPROVE disable/enable find button if text/path is valid: freedom vs validation
    createButton = new QPushButton{tr("&Create Library and Index Documents")};
    createButton->setDefault(true);
    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    QObject::connect(
        findDirectoryButton, SIGNAL(clicked()),
        this, SLOT(handleFindDirectory()));
    QObject::connect(
        closeButton, SIGNAL(clicked()),
        this, SLOT(close()));

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{};
    QHBoxLayout* srcButtonLayout = new QHBoxLayout{};
    mainLayout->addWidget(findLibrarySourceLabel);
    srcButtonLayout->addWidget(findDirectoryButton);
    srcButtonLayout->addStretch();
    mainLayout->addLayout(srcButtonLayout);
    mainLayout->addWidget(uriLabel);
    mainLayout->addWidget(uriEdit);
    mainLayout->addWidget(pdfCheckBox);

    QHBoxLayout* buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(createButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Add Document Library"));
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

AddLibraryDialog::~AddLibraryDialog()
{
    delete findLibrarySourceLabel;
    delete uriLabel;
    delete uriEdit;
    delete findDirectoryButton;
    delete pdfCheckBox;
    delete createButton;
    delete closeButton;
}

void AddLibraryDialog::show()
{
    QDialog::show();
}

void AddLibraryDialog::handleFindDirectory()
{
    QString homeDirectory
        = QStandardPaths::locate(
            QStandardPaths::HomeLocation,
            QString(),
            QStandardPaths::LocateDirectory);

    QFileDialog fileDialog{this};
    fileDialog.setWindowTitle(tr("Choose Directory"));
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setDirectory(homeDirectory);
    fileDialog.setViewMode(QFileDialog::Detail);

    QList<QString> fileNames{};
    if(fileDialog.exec()) {
        fileNames = fileDialog.selectedFiles();
        if(fileNames.size()==1) {
            uriEdit->setText(fileNames[0]);
        } // else too many files
    } // else directory closed / nothing choosen
}

} // m8r namespace
