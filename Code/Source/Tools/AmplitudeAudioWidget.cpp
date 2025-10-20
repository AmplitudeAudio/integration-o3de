// Copyright (c) 2025-present Sparky Studios. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <AzCore/Utils/Utils.h>

#include <QLabel>
#include <QVBoxLayout>

#include "AmplitudeAudioWidget.h"

namespace SparkyStudios::Audio::Amplitude
{
    AmplitudeAudioWidget::AmplitudeAudioWidget(QWidget* parent)
        : QWidget(parent)
    {
        QVBoxLayout* mainLayout = new QVBoxLayout(this);

        QLabel* introLabel = new QLabel(QObject::tr("Put your cool stuff here!"), this);
        mainLayout->addWidget(introLabel, 0, Qt::AlignCenter);

        QString helpText = QString(
            "For help getting started, visit the <a href=\"https://o3de.org/docs/tools-ui/\">UI Development</a> documentation<br/>or come "
            "ask a question in the <a href=\"https://discord.gg/R77Wss3kHe\">sig-ui-ux channel</a> on Discord");

        QLabel* helpLabel = new QLabel(this);
        helpLabel->setTextFormat(Qt::RichText);
        helpLabel->setText(helpText);
        helpLabel->setOpenExternalLinks(true);

        mainLayout->addWidget(helpLabel, 0, Qt::AlignCenter);

        setLayout(mainLayout);
    }
} // namespace SparkyStudios::Audio::Amplitude

#include <moc_AmplitudeAudioWidget.cpp>
