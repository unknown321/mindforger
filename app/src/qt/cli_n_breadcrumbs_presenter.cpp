/*
 cli_n_breadcrumbs_presenter.cpp     MindForger thinking notebook

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
#include "cli_n_breadcrumbs_presenter.h"

using namespace std;

namespace m8r {

CliAndBreadcrumbsPresenter::CliAndBreadcrumbsPresenter(
        MainWindowPresenter* mainPresenter,
        CliAndBreadcrumbsView* view,
        Mind* mind)
    : mainPresenter(mainPresenter), view(view), mind(mind)
{
    // widgets
    view->setVisible(Configuration::getInstance().isUiShowBreadcrump());

    // wire signals (view events to presenter handlers)
    QObject::connect(
        view->cli, SIGNAL(returnPressed()),
        this, SLOT(executeCommand()));
    QObject::connect(
        view->cli, SIGNAL(textChanged(QString)),
        this, SLOT(handleCliTextChanged(QString)));
}


void CliAndBreadcrumbsPresenter::handleCliTextChanged(const QString& text)
{
    // IMPROVE remove parameter text if it's not needed
    UNUSED_ARG(text);

    // TODO use status bar

    QString command = view->getCommand();
    MF_DEBUG("CLI text changed to: '" << command.toStdString() << "'" << endl);
    if(command.size()) {
        MF_DEBUG("  handling:" << endl);
        if(command.startsWith(CliAndBreadcrumbsView::CHAR_HELP)) {
            MF_DEBUG("    HELP" << endl);
            QMessageBox::information(
                &mainPresenter->getView(),
                tr("Wingman help"),
                tr(
                    // IMPROVE consider <html> prefix, <br/> separator and colors/bold
                    "<html>"
                    "Use the following commands:"
                    "<pre>"
                    "<br>? ... help"
                    "<br>/ ... find"
                    "<br>@ ... knowledge recherche"
                    "<br>> ... run a command"
                    //"<br>: ... chat with workspace, Notebook or Note"
                    "<br>&nbsp;&nbsp;... or full-text search phrase"
                    "</pre>"
                    "<br>Examples:"
                    "<pre>"
                    "<br>/ find notebook by tag TODO"
                    "<br>@arxiv LLM"
                    "<br>> emojis"
                    //"<br>: explain in simple terms SELECTED"
                    "</pre>"
                )
            );
            view->setCommand("");
            mainPresenter->getStatusBar()->showInfo(
                tr("Wingman: ? for help, / search, @ knowledge, > command, or type FTS phrase"));
            return;
        } else if(command.startsWith(CliAndBreadcrumbsView::CHAR_FIND)) {
            MF_DEBUG("    / HELP find" << endl);
            if(command.size()<=2) {
                view->updateCompleterModel(CliAndBreadcrumbsView::HELP_FIND_CMDS);
            }
            return;
        } else if(command.startsWith(CliAndBreadcrumbsView::CHAR_KNOW)) {
            MF_DEBUG("    @ HELP knowledge" << endl);
            if(command.size()<=2) {
                view->updateCompleterModel(view->HELP_KNOW_CMDS);
            }
            return;
        } else if(command.startsWith(CliAndBreadcrumbsView::CHAR_CMD)) {
            MF_DEBUG("    > HELP command" << endl);
            if(command.size()<=2) {
                view->updateCompleterModel(CliAndBreadcrumbsView::HELP_CMD_CMDS);
            }
            return;
        } else if(command.startsWith(CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME)) {
            QString prefix(
                QString::fromStdString(
                    command.toStdString().substr(
                        CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME.size()-1)));
            if(prefix.size()) {
                mainPresenter->getStatusBar()->showInfo(prefix);
                if(prefix.size()==1) {
                    // switch suggestions model
                    vector<string> outlineNames;
                    mind->getOutlineNames(outlineNames);
                    QList<QString> outlineNamesCompletion = QList<QString>();
                    if(outlineNames.size()) {
                        QString qs;
                        for(const string& s:outlineNames) {
                            qs.clear();
                            // TODO commands are constants
                            qs += CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME;
                            qs += QString::fromStdString(s);
                            outlineNamesCompletion << qs;
                        }
                    }
                    view->updateCompleterModel(
                        CliAndBreadcrumbsView::DEFAULT_CMDS,
                        &outlineNamesCompletion);
                } else {
                    // TODO NOT handled
                }
            } else {
                MF_DEBUG("    FALLBACK (default CMDs)" << endl);
                view->updateCompleterModel(
                    CliAndBreadcrumbsView::DEFAULT_CMDS);
            }
            return;
        }
        MF_DEBUG("    NO HANDLING (FTS phrase OR lost focus)" << endl);
        return;
    } else { // empty command
        MF_DEBUG("    EMPTY command > NO handling" << endl);
        return;
    }
}

// TODO i18n
void CliAndBreadcrumbsPresenter::executeCommand()
{
    QString command = view->getCommand();
    MF_DEBUG("CLI command EXEC: '" << command.toStdString() << "'" << endl);
    if(command.size()) {
        view->addCompleterItem(command);

        if(command.startsWith(CliAndBreadcrumbsView::CMD_LIST_OUTLINES)) {
            MF_DEBUG("  executing: list outlines" << endl);
            executeListOutlines();
            view->showBreadcrumb();
            return;
        } else if(command.startsWith(CliAndBreadcrumbsView::CMD_EMOJIS)) {
            MF_DEBUG("  executing: emojis" << endl);
            mainPresenter->doActionEmojisDialog();
            return;
        } else if(command.startsWith(CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_TAG)) {
            string name = command.toStdString().substr(
                CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_TAG.size());
            MF_DEBUG("  executing: find O by tag '" << name << "'" << endl);
            if(name.size()) {
                mainPresenter->doActionFindOutlineByTag(name);
            } else {
                mainPresenter->getStatusBar()->showInfo(tr("Notebook not found - please specify tag search phrase (is empty)"));
            }
            return;
        } else if(command.startsWith(CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME)) {
            string name = command.toStdString().substr(
                CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME.size());
            MF_DEBUG("  executing: find O by name '" << name << "'" << endl);
            if(name.size()) {
                mainPresenter->doActionFindOutlineByName(name);
            } else {
                mainPresenter->getStatusBar()->showInfo(tr("Notebook not found - please specify name search phrase (is empty)"));
            }
            // status handling examples:
            // mainPresenter->getStatusBar()->showInfo(tr("Notebook ")+QString::fromStdString(outlines->front()->getName()));
            // mainPresenter->getStatusBar()->showInfo(tr("Notebook not found: ") += QString(name.c_str()));
            return;
        } else
        // knowledge lookup in the CLI:
        // - @wikipedia     ... opens tool dialog with Wikipedi SELECTED in the dropdown
        // - @wikipedia llm ... opens directly https://wikipedia.org
        if(command.startsWith(CliAndBreadcrumbsView::CHAR_KNOW)) {
            for(auto c:view->HELP_KNOW_CMDS) {
                QString toolId = command.mid(1, c.size()-1);
                if(command.startsWith(c)) {
                    QString phrase = command.mid(1 + c.size());
                    MF_DEBUG(
                        "  executing: knowledge recherche of phrase '"
                        << phrase.toStdString()
                        << "' using command '"
                        << c.toStdString() << "' and tool '"
                        << toolId.toStdString() << "'" << endl);
                    if(phrase.size() > 1) {
                        mainPresenter->doActionOpenRunToolDialog(phrase, toolId, false);
                        mainPresenter->handleRunTool();
                    } else {
                        // search phrase is empty
                        mainPresenter->doActionOpenRunToolDialog(phrase, toolId);
                    }
                    return;
                }
            }
            // ELSE: unknown @unknown knowledge recherche tool do FTS as fallback
            mainPresenter->getStatusBar()->showInfo(tr("Unknown knowledge recherche source - use valid source like @wikipedia"));
        } else {
            mainPresenter->doFts(view->getCommand(), true);
        }
    } else {
        mainPresenter->getStatusBar()->showError(tr("No command!"));
    }
}

void CliAndBreadcrumbsPresenter::executeListOutlines()
{
    mainPresenter->getOrloj()->showFacetOutlineList(mind->getOutlines());
}

// TODO call main window handler
void CliAndBreadcrumbsPresenter::executeListNotes()
{
    if(mainPresenter->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
         ||
       mainPresenter->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)
         ||
       mainPresenter->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)
         ||
       mainPresenter->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)
         ||
       mainPresenter->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)
    ) {
        Outline* o = mainPresenter->getOrloj()->getOutlineView()->getCurrentOutline();
        if(o) {
            auto notes = o->getNotes();
            // TODO push notes to CLI completer > filter > ENTER outline's note in view
            if(!notes.empty()) {
                mainPresenter->getOrloj()->showFacetNoteEdit(notes.front());
            }
        }

        // TODO show note on enter (actions handler)
        mainPresenter->getStatusBar()->showInfo(QString("Listing notes..."));
        return;
    }
    // else show all notes in MF (scalability?)
    mainPresenter->getStatusBar()->showInfo(QString("No notes to list!"));
}

}
