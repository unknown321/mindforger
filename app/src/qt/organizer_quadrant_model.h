/*
 organizer_quadrant_model.h     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef M8RUI_ORGANIZER_QUADRANT_MODEL_H
#define M8RUI_ORGANIZER_QUADRANT_MODEL_H

#include "../../lib/src/representations/unicode.h"
#include "../../lib/src/representations/html/html_outline_representation.h"

#include <QtWidgets>

#include "model_meta_definitions.h"

namespace m8r {

class OrganizerQuadrantModel : public QStandardItemModel
{
    Q_OBJECT

    QString title;

    HtmlOutlineRepresentation* htmlRepresentation;

public:
    explicit OrganizerQuadrantModel(
        QString& title,
        QObject* parent,
        HtmlOutlineRepresentation* htmlRepresentation
    );
    OrganizerQuadrantModel(const OrganizerQuadrantModel&) = delete;
    OrganizerQuadrantModel(const OrganizerQuadrantModel&&) = delete;
    OrganizerQuadrantModel& operator =(const OrganizerQuadrantModel&) = delete;
    OrganizerQuadrantModel& operator =(const OrganizerQuadrantModel&&) = delete;
    ~OrganizerQuadrantModel();

    void setTitle(QString& title) {
        this->title = title;
        QList<QString> tableHeader = {this->title};
        setHorizontalHeaderLabels(tableHeader);
    }

    void addRow(Note* note, bool urgency=false, bool importance=false, bool showOutline=true);
    void removeAllRows();
};

}
#endif // M8RUI_ORGANIZER_QUADRANT_MODEL_H
