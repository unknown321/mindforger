# app.pro     Qt project file for MindForger Qt-based frontend
#
# Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

TARGET = mindforger
TEMPLATE = app

message("= MindForger QMake configuration ==========================")
message("Qt version: $$QT_VERSION")

QT += widgets

win32 {
  QT += network
}

mfdebug|mfunits {
  DEFINES += DO_MF_DEBUG
}

mfci {
  DEFINES += DO_MF_CI
}

mfrc {
  DEFINES += DO_MF_RC
}

# Hunspell spell check:
# - Windows and Ubuntu Xenial require DEPRECATED Hunspell API
# - Ubuntu Bionic and newer distros use NEW Hunspell API
# - macOS doesn't care as it uses different provider
# Distro is detected on Unix/Linux only:
unix {
  !macx {
    OS_DISTRO_VERSION = $$system(cat /etc/issue | while read D V X; do echo "${D} ${V}"; done | rev | cut -c 3- | rev)
  } else {
    OS_DISTRO_VERSION = "macOS"
  }
} else {
  OS_DISTRO_VERSION = "Windows"
}
message("OS version: $$OS_DISTRO_VERSION")

mfoldhunspell | equals(OS_DISTRO_VERSION, "Windows") | equals(OS_DISTRO_VERSION, "Ubuntu 16.04") | equals(OS_DISTRO_VERSION, "Ubuntu 16.") | equals(OS_DISTRO_VERSION, "Debian GNU/Lin") {
  message("Hunspell: forcing LEGACY API on OS: $$OS_DISTRO_VERSION")
  DEFINES += MF_DEPRECATED_HUNSPELL_API
} else {
  message("Hunspell: configuring use of NEW API on OS: $$OS_DISTRO_VERSION")
}

# webkit is supposed to be OBSOLETED by webengine, but webengine is disabled
# on Linux since Qt 5.9 due to its tragic performance -> conditional compilation
# seems to be the only way:
# - webkit on Linux
# - webengine on Windows and macOS
win32|macx|mfwebengine {
    DEFINES += MF_QT_WEB_ENGINE
    QT += webenginecore
    QT += webenginewidgets
} else {
    QT += webkit
    QT += webkitwidgets
}

# Dependencies:
# - INCLUDEPATH is used during compilation to find included header files.
# - DEPENDPATH is used to resolve dependencies between header and source
#   files, e.g. which source files need to be recompiled when certain header
#   file changes.
INCLUDEPATH += $$PWD/../lib/src
DEPENDPATH += $$PWD/../lib/src

# -L where to look for library, -l link the library

# MindForger lib
win32 {
    CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/release -lmindforger
    else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/debug -lmindforger
} else {
    # Linux and macOS
    # TODO split macOS
    LIBS += -L$$OUT_PWD/../lib -lmindforger -lcurl
}

# Markdown to HTML: cmark-gfm (or nothing)
!mfnomd2html {
  # cmark-gfm
  DEFINES += MF_MD_2_HTML_CMARK

  win32 {
    message("cmark-gfm: ensure that cmark-gfm was MANUALLY built BEFORE qmake run on Windows")
    CONFIG(release, debug|release) {
      LIBS += -L$$PWD/../deps/cmark-gfm/build/src/Release -lcmark-gfm_static
      LIBS += -L$$PWD/../deps/cmark-gfm/build/extensions/Release -lcmark-gfm-extensions_static
    } else:CONFIG(debug, debug|release) {
      LIBS += -L$$PWD/../deps/cmark-gfm/build/src/Debug -lcmark-gfm_static
      LIBS += -L$$PWD/../deps/cmark-gfm/build/extensions/Debug -lcmark-gfm-extensions_static
    }
  } else {
    message("cmark-gfm: ensure that cmark-gfm was MANUALLY built BEFORE qmake run on Linux/macOS")
    # cmark-gfm to be built by qmake to enable clean system build for Launchpad debuild
    libcmark-gfm.target = libcmark-gfm
    libcmark-gfm.commands = cd -L$$PWD/../deps/cmark-gfm && mkdir -v build && cd build && cmake -DCMARK_TESTS=OFF -DCMARK_SHARED=OFF .. && cmake --build .
    libcmark-gfm_clean.commands = cd -L$$PWD/../deps/cmark-gfm rm -rvf build
    QMAKE_EXTRA_TARGETS += libcmark-gfm

    LIBS += -L$$PWD/../deps/cmark-gfm/build/extensions -lcmark-gfm-extensions
    LIBS += -L$$PWD/../deps/cmark-gfm/build/src -lcmark-gfm
  }
}

# Zlib
win32 {
    INCLUDEPATH += $$PWD/../deps/zlib-win/include
    DEPENDPATH += $$PWD/../deps/zlib-win/include

    CONFIG(release, debug|release): LIBS += -L$$PWD/../deps/zlib-win/lib/ -lzlibwapi
    else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../deps/zlib-win/lib/ -lzlibwapi
} else {
    LIBS += -lz
}

win32 {
    LIBS += -lRpcrt4 -lOle32
}

# spellcheck
macx {
    HEADERS += ./src/qt/spelling/dictionary_provider_nsspellchecker.h

    OBJECTIVE_SOURCES += src/qt/spelling/dictionary_provider_nsspellchecker.mm

    LIBS += -framework AppKit
    LIBS += -framework Foundation
} else:win32 {
    include(../deps/hunspell/hunspell.pri)

    HEADERS += \
      ./src/qt/spelling/dictionary_provider_hunspell.h \
      ./src/qt/spelling/dictionary_provider_voikko.h

    SOURCES += \
      ./src/qt/spelling/dictionary_provider_hunspell.cpp \
      ./src/qt/spelling/dictionary_provider_voikko.cpp

} else:unix {
    # pkgconfig-based configuration does not work @ Ubuntu
    #  CONFIG += link_pkgconfig
    #  PKGCONFIG += hunspell

    # hardcoded paths are (unfortunately) more robust:
    INCLUDEPATH += /usr/include/hunspell
    LIBS += -lhunspell

    HEADERS += \
      ./src/qt/spelling/dictionary_provider_hunspell.h \
      ./src/qt/spelling/dictionary_provider_voikko.h

    SOURCES += \
      ./src/qt/spelling/dictionary_provider_hunspell.cpp \
      ./src/qt/spelling/dictionary_provider_voikko.cpp
}
INCLUDEPATH += ./src/qt/spelling

# development environment remarks:
# - Mind  64b: GCC 7.5.0, Qt 5.9.5
# - Beast 64b: GCC 5.4.0, Qt 5.5.1
# - S7    64b: GCC 4.8.5, Qt 5.2.1
# - Win10 64b: MSVC 2017, Qt 5.12.0
#
# - GCC: -std=c++0x ~ -std=c++11
#
# compiler and compiler options:
#   - https://doc.qt.io/qt-6/qmake-variable-reference.html#qmake-cxx
#   - QMAKE_CXX       ... compiler
#   - QMAKE_CXXFLAGS  ... compiler options
#   - compilation options ~ DEFINEs (qmake CONFIG+=mfnoccache ...)
#
win32{
    QMAKE_CXXFLAGS += /MP

    # DISABLED ccache as it causes compilation error:
    #   "C1090: PDB API call failed, error code '23'" when used
    # when used w/ MS VS compiler:
    # !mfnoccache { QMAKE_CXX = ccache $$QMAKE_CXX }
} else {
    # linux and macos
    mfnoccache {
      QMAKE_CXX = g++
    } else:!mfnocxx {
      QMAKE_CXX = ccache g++
    }
    QMAKE_CXXFLAGS += -pedantic -std=c++11
}

# profiling: instrument code for gprof
#QMAKE_CXXFLAGS_DEBUG *= -pg
#QMAKE_LFLAGS_DEBUG *= -pg

HEADERS += \
    ./src/qt/navigator/edge.h \
    ./src/qt/navigator/navigator_view.h \
    ./src/qt/navigator/node.h \
    ./src/qt/main_window_view.h \
    ./src/qt/outlines_table_view.h \
    ./src/qt/outlines_table_presenter.h \
    ./src/qt/outline_tree_view.h \
    ./src/qt/outline_tree_presenter.h \
    ./src/qt/status_bar_view.h \
    ./src/qt/status_bar_presenter.h \
    ./src/qt/main_window_presenter.h \
    ./src/qt/main_menu_view.h \
    ./src/qt/main_menu_presenter.h \
    ./src/qt/cli_n_breadcrumbs_presenter.h \
    ./src/qt/cli_n_breadcrumbs_view.h \
    ./src/qt/orloj_view.h \
    ./src/qt/orloj_presenter.h \
    ./src/qt/notes_table_view.h \
    ./src/qt/notes_table_presenter.h \
    ./src/qt/notes_table_model.h \
    ./src/qt/outline_tree_model.h \
    ./src/qt/outlines_table_model.h \
    ./src/qt/model_meta_definitions.h \
    ./src/qt/note_view_model.h \
    ./src/qt/note_view_presenter.h \
    ./src/qt/note_view.h \
    ./src/qt/note_edit_presenter.h \
    ./src/qt/look_n_feel.h \
    ./src/qt/html_delegate.h \
    ./src/qt/note_edit_highlighter.h \
    ./src/qt/gear/qutils.h \
    ./src/qt/i18nl10n.h \
    ./src/qt/outline_view_presenter.h \
    ./src/qt/outline_view.h \
    ./src/qt/outline_header_edit_presenter.h \
    ./src/qt/outline_header_view_presenter.h \
    ./src/qt/outline_header_edit_view.h \
    ./src/qt/outline_header_view.h \
    ./src/qt/note_editor_view.h \
    ./src/qt/note_edit_view.h \
    ./src/qt/dialogs/note_new_dialog.h \
    ./src/qt/dialogs/outline_new_dialog.h \
    ./src/qt/dialogs/fts_dialog.h \
    ./src/qt/dialogs/find_outline_by_name_dialog.h \
    ./src/qt/dialogs/find_note_by_name_dialog.h \
    ./src/qt/dialogs/note_edit_dialog.h \
    ./src/qt/dialogs/configuration_dialog.h \
    ./src/qt/widgets/edit_tags_panel.h \
    ./src/qt/widgets/labeled_edit_line_panel.h \
    ./src/qt/widgets/line_number_panel.h \
    ./src/qt/dialogs/outline_header_edit_dialog.h \
    ./src/qt/widgets/urgency_combo_box.h \
    ./src/qt/widgets/importance_combo_box.h \
    ./src/qt/dialogs/refactor_note_to_outline_dialog.h \
    ./src/qt/recent_files_menu.h \
    ./src/qt/dialogs/scope_dialog.h \
    ./src/qt/dialogs/find_outline_by_tag_dialog.h \
    ./src/qt/dialogs/find_note_by_tag_dialog.h \
    ./src/qt/outline_view_splitter.h \
    ./src/qt/assoc_leaderboard_model.h \
    ./src/qt/assoc_leaderboard_presenter.h \
    ./src/qt/assoc_leaderboard_view.h \
    ./src/qt/gear/async_task_notifications_distributor.h \
    ./src/qt/dialogs/insert_image_dialog.h \
    ./src/qt/dialogs/insert_link_dialog.h \
    ./src/qt/dialogs/rows_and_depth_dialog.h \
    ./src/qt/dialogs/new_file_dialog.h \
    ./src/qt/dialogs/new_repository_dialog.h \
    src/qt/dialogs/add_library_dialog.h \
    src/qt/dialogs/export_csv_file_dialog.h \
    src/qt/dialogs/organizer_new_dialog.h \
    src/qt/dialogs/rm_library_dialog.h \
    src/qt/dialogs/run_tool_dialog.h \
    src/qt/dialogs/wingman_dialog.h \
    src/qt/dialogs/sync_library_dialog.h \
    src/qt/dialogs/terminal_dialog.h \
    src/qt/kanban_column_model.h \
    src/qt/kanban_column_presenter.h \
    src/qt/kanban_column_view.h \
    src/qt/kanban_presenter.h \
    src/qt/kanban_view.h \
    src/qt/note_smart_editor.h \
    src/qt/organizer_view.h \
    src/qt/organizer_presenter.h \
    src/qt/organizer_quadrant_model.h \
    src/qt/organizer_quadrant_presenter.h \
    src/qt/organizer_quadrant_view.h \
    src/qt/organizers_table_model.h \
    src/qt/organizers_table_presenter.h \
    src/qt/organizers_table_view.h \
    src/qt/outlines_map_model.h \
    src/qt/outlines_map_presenter.h \
    src/qt/outlines_map_view.h \
    src/qt/qt_commons.h \
    src/qt/spelling/abstract_dictionary.h \
    src/qt/spelling/abstract_dictionary_provider.h \
    src/qt/spelling/dictionary_manager.h \
    src/qt/spelling/dictionary_ref.h \
    src/qt/spelling/spell_checker.h \
    src/qt/tags_table_model.h \
    src/qt/tags_table_presenter.h \
    src/qt/tags_table_view.h \
    src/qt/recent_notes_table_model.h \
    src/qt/recent_notes_table_presenter.h \
    src/qt/recent_notes_table_view.h \
    src/qt/navigator_presenter.h \
    src/qt/main_toolbar_view.h \
    src/qt/dialogs/export_file_dialog.h \
    src/qt/widgets/edit_buttons_panel.h \
    src/qt/widgets/edit_name_panel.h \
    src/qt/widgets/view_to_edit_buttons_panel.h \
    src/qt/widgets/mf_widgets.h \
    src/qt/outline_header_view_model.h \
    src/qt/dialogs/fts_dialog_presenter.h \
    src/qt/gear/apple_utils.h

win32|macx|mfwebengine {
    HEADERS += ./src/qt/web_engine_page_link_navigation_policy.h
}

SOURCES += \
    ./src/qt/mindforger.cpp \
    ./src/qt/navigator/edge.cpp \
    ./src/qt/navigator/navigator_view.cpp \
    ./src/qt/navigator/node.cpp \
    ./src/qt/main_window_view.cpp \
    ./src/qt/outlines_table_view.cpp \
    ./src/qt/outlines_table_presenter.cpp \
    ./src/qt/outline_tree_view.cpp \
    ./src/qt/outline_tree_presenter.cpp \
    ./src/qt/status_bar_view.cpp \
    ./src/qt/status_bar_presenter.cpp \
    ./src/qt/main_window_presenter.cpp \
    ./src/qt/main_menu_view.cpp \
    ./src/qt/main_menu_presenter.cpp \
    ./src/qt/cli_n_breadcrumbs_presenter.cpp \
    ./src/qt/cli_n_breadcrumbs_view.cpp \
    ./src/qt/orloj_view.cpp \
    ./src/qt/orloj_presenter.cpp \
    ./src/qt/notes_table_view.cpp \
    ./src/qt/notes_table_presenter.cpp \
    ./src/qt/notes_table_model.cpp \
    ./src/qt/outline_tree_model.cpp \
    ./src/qt/outlines_table_model.cpp \
    ./src/qt/note_view_model.cpp \
    ./src/qt/note_view_presenter.cpp \
    ./src/qt/note_view.cpp \
    ./src/qt/note_edit_presenter.cpp \
    ./src/qt/look_n_feel.cpp \
    ./src/qt/html_delegate.cpp \
    ./src/qt/note_edit_highlighter.cpp \
    ./src/qt/gear/qutils.cpp \
    ./src/qt/i18nl10n.cpp \
    ./src/qt/outline_view_presenter.cpp \
    ./src/qt/outline_view.cpp \
    ./src/qt/outline_header_edit_presenter.cpp \
    ./src/qt/outline_header_view_presenter.cpp \
    ./src/qt/outline_header_edit_view.cpp \
    ./src/qt/outline_header_view.cpp \
    ./src/qt/note_editor_view.cpp \
    ./src/qt/note_edit_view.cpp \
    ./src/qt/dialogs/note_new_dialog.cpp \
    ./src/qt/dialogs/outline_new_dialog.cpp \
    ./src/qt/dialogs/fts_dialog.cpp \
    ./src/qt/dialogs/find_outline_by_name_dialog.cpp \
    ./src/qt/dialogs/find_note_by_name_dialog.cpp \
    ./src/qt/dialogs/note_edit_dialog.cpp \
    ./src/qt/dialogs/configuration_dialog.cpp \
    ./src/qt/widgets/edit_tags_panel.cpp \
    ./src/qt/widgets/labeled_edit_line_panel.cpp \
    ./src/qt/widgets/line_number_panel.cpp \
    ./src/qt/dialogs/outline_header_edit_dialog.cpp \
    ./src/qt/widgets/urgency_combo_box.cpp \
    ./src/qt/widgets/importance_combo_box.cpp \
    ./src/qt/dialogs/refactor_note_to_outline_dialog.cpp \
    ./src/qt/recent_files_menu.cpp \
    ./src/qt/dialogs/scope_dialog.cpp \
    ./src/qt/dialogs/find_outline_by_tag_dialog.cpp \
    ./src/qt/dialogs/find_note_by_tag_dialog.cpp \
    ./src/qt/outline_view_splitter.cpp \
    ./src/qt/assoc_leaderboard_model.cpp \
    ./src/qt/assoc_leaderboard_presenter.cpp \
    ./src/qt/assoc_leaderboard_view.cpp \
    ./src/qt/gear/async_task_notifications_distributor.cpp \
    ./src/qt/dialogs/insert_image_dialog.cpp \
    ./src/qt/dialogs/insert_link_dialog.cpp \
    ./src/qt/dialogs/rows_and_depth_dialog.cpp \
    ./src/qt/dialogs/new_file_dialog.cpp \
    ./src/qt/dialogs/new_repository_dialog.cpp \
    src/qt/dialogs/add_library_dialog.cpp \
    src/qt/dialogs/export_csv_file_dialog.cpp \
    src/qt/dialogs/organizer_new_dialog.cpp \
    src/qt/dialogs/rm_library_dialog.cpp \
    src/qt/dialogs/run_tool_dialog.cpp \
    src/qt/dialogs/wingman_dialog.cpp \
    src/qt/dialogs/sync_library_dialog.cpp \
    src/qt/dialogs/terminal_dialog.cpp \
    src/qt/kanban_column_model.cpp \
    src/qt/kanban_column_presenter.cpp \
    src/qt/kanban_column_view.cpp \
    src/qt/kanban_presenter.cpp \
    src/qt/kanban_view.cpp \
    src/qt/note_smart_editor.cpp \
    src/qt/organizer_view.cpp \
    src/qt/organizer_presenter.cpp \
    src/qt/organizer_quadrant_model.cpp \
    src/qt/organizer_quadrant_presenter.cpp \
    src/qt/organizer_quadrant_view.cpp \
    src/qt/organizers_table_model.cpp \
    src/qt/organizers_table_presenter.cpp \
    src/qt/organizers_table_view.cpp \
    src/qt/outlines_map_model.cpp \
    src/qt/outlines_map_presenter.cpp \
    src/qt/outlines_map_view.cpp \
    src/qt/spelling/dictionary_manager.cpp \
    src/qt/spelling/spell_checker.cpp \
    src/qt/tags_table_model.cpp \
    src/qt/tags_table_presenter.cpp \
    src/qt/tags_table_view.cpp \
    src/qt/recent_notes_table_model.cpp \
    src/qt/recent_notes_table_presenter.cpp \
    src/qt/recent_notes_table_view.cpp \
    src/qt/navigator_presenter.cpp \
    src/qt/main_toolbar_view.cpp \
    src/qt/dialogs/export_file_dialog.cpp \
    src/qt/widgets/edit_buttons_panel.cpp \
    src/qt/widgets/edit_name_panel.cpp \
    src/qt/widgets/view_to_edit_buttons_panel.cpp \
    src/qt/widgets/mf_widgets.cpp \
    src/qt/outline_header_view_model.cpp \
    src/qt/dialogs/fts_dialog_presenter.cpp

win32|macx|mfwebengine {
    SOURCES += ./src/qt/web_engine_page_link_navigation_policy.cpp
}

win32 {
    HEADERS += \
    ../deps/getopt/getopt.h

    SOURCES += \
    ../deps/getopt/getopt.c
}

# L10n
#./resources/qt/translations/mindforger_de.ts
#./resources/qt/translations/mindforger_fr.ts
#./resources/qt/translations/mindforger_es.ts
TRANSLATIONS = \
    ./resources/qt/translations/mindforger_nerd_en.ts \
    ./resources/qt/translations/mindforger_nerd_cs.ts \
    ./resources/qt/translations/mindforger_en.ts \
    ./resources/qt/translations/mindforger_cs.ts \
    ./resources/qt/translations/mindforger_zh_cn.ts

RESOURCES += \
    ./mf-resources.qrc

# ########################################
# Linux installation: make install
# ########################################
# See http://doc.qt.io/qt-5/qmake-advanced-usage.html

binfile.files += mindforger
binfile.path = $$PREFIX/bin/
INSTALLS += binfile

# ########################################
# macOS DMG content
# ########################################

macx {
    # ask for APP BUNDLE generation
    CONFIG += app_bundle  # to remove, do CONFIG -= app_bundle

    ICON = $$PWD/resources/icons/mindforger.icns
    #ICON = app/resources/icons/mindforger.icns

    macosdocfiles.files = $$PWD/../doc/limbo $$PWD/../doc/memory $$PWD/../doc/stencils $$PWD/../doc/README.md
    macosdocfiles.path = Contents/Resources/mindforger-repository
    QMAKE_BUNDLE_DATA += macosdocfiles
}

# ########################################
# Windows .exe content
# ########################################

win32 {
    # icon and exe detail information
    RC_FILE = $$PWD/resources/windows/mindforger.rc
}

# ########################################
# Diagnostics
# ########################################

message(DEFINES of app.pro build: $$DEFINES)
message(QMAKE_EXTRA_TARGETS of app.pro build: $$QMAKE_EXTRA_TARGETS)
message(QT of app.pro build: $$QT)
message(PATH is: $$(PATH))

# eof
