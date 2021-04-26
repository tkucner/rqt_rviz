/*
 * Copyright (c) 2018, Open Source Robotics Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the TU Darmstadt nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <dirent.h>
#include <QComboBox>

#include <rqt_rviz/config_dialog.h>

#include <iostream>

namespace rqt_rviz {

    bool has_suffix(const std::string& s, const std::string& suffix)
{
    return (s.size() >= suffix.size()) && equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}

    ConfigDialog::ConfigDialog() {


        // Window configurations
        this->setWindowTitle(tr("Choose configuration"));
        this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
                             Qt::WindowStaysOnTopHint |
                             Qt::CustomizeWindowHint);

        // File
        QLabel *file_label = new QLabel("File path");
        file_label->setToolTip("Full path to file");

        source_dir_ = new QLineEdit;
        source_dir_->setMinimumWidth(300);





        QPushButton *browse_button = new QPushButton(tr("Browse"));
        connect(browse_button, SIGNAL(clicked()), this, SLOT(OnBrowse()));

        // Hide menu
        QLabel *hide_label = new QLabel("Hide menu");
        hide_label->setToolTip("Check to hide RViz's top menu bar");

        hide_box_ = new QCheckBox();

        // Buttons
        QPushButton *cancel_button = new QPushButton(tr("&Cancel"));
        this->connect(cancel_button, SIGNAL(clicked()), this, SLOT(close()));

        QPushButton *apply_button = new QPushButton(tr("&Apply"));
        apply_button->setDefault(true);
        this->connect(apply_button, SIGNAL(clicked()), this, SLOT(accept()));

        QHBoxLayout *buttons_layout = new QHBoxLayout;
        buttons_layout->addWidget(cancel_button);
        buttons_layout->addWidget(apply_button);

        // Dropdown list
        config_list = new QComboBox();
        this->connect(config_list, SIGNAL(activated(int)),this,SLOT(OnSelect()));
        QLabel *select_configuration = new QLabel("Select Configuration");
        // Layout
        QGridLayout *main_layout = new QGridLayout();

        main_layout->addWidget(file_label, 0, 0);
        main_layout->addWidget(source_dir_, 0, 1);
        main_layout->addWidget(browse_button, 0, 2);


        main_layout->addWidget(select_configuration, 1, 0);
        main_layout->addWidget(config_list, 1, 1);

        main_layout->addWidget(hide_label, 2, 0);
        main_layout->addWidget(hide_box_, 2, 1);
        main_layout->setAlignment(hide_box_, Qt::AlignLeft);

        main_layout->addLayout(buttons_layout, 2, 0, 1, 3);
        main_layout->setColumnStretch(1, 2);


        this->setLayout(main_layout);
    }

    ConfigDialog::~ConfigDialog() {
    }

    void ConfigDialog::OnSelect() {
        file_edit_=source_dir_->text().toStdString()+"/"+config_list->currentText().toStdString();

    }

    void ConfigDialog::OnBrowse() {
        std::vector<std::string> files;
        QString filename = QFileDialog::getExistingDirectory(0,
                                                             tr("Choose config file:"),
                                                             "");

        source_dir_->setText(filename);
        std::string directory=filename.toUtf8().constData();
        files=get_config_files(directory);
        for (int i=0;i<files.size();i++)
            config_list->addItem(QString::fromStdString(files[i]));

    }

    std::string ConfigDialog::GetFile() const {
        return file_edit_;
    }

    void ConfigDialog::SetFile(const std::string &file) {
        std::string tr_file= file.substr(0, file.rfind("/"));
        source_dir_->setText(QString::fromStdString(tr_file));


        std::vector<std::string> files=get_config_files(tr_file);
        for (int i=0;i<files.size();i++)
            config_list->addItem(QString::fromStdString(files[i]));

    }

    bool ConfigDialog::GetHide() const {
        return hide_box_->isChecked();
    }

    void ConfigDialog::SetHide(const bool hide) {
        hide_box_->setChecked(hide);
    }

    std::vector<std::string> ConfigDialog::get_config_files(std::string path) {
        std::vector<std::string> files;
        DIR *dir;

        struct dirent *ent;
        if ((dir = opendir(path.c_str())) != NULL) {
            /* print all the files and directories within directory */
            while ((ent = readdir(dir)) != NULL) {

                if(has_suffix(ent->d_name, ".rviz"))
                    files.push_back(ent->d_name);
            }
            closedir(dir);
        } else {
            /* could not open directory */
            perror("");

        }
        return files;
    }
}

