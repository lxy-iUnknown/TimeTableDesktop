#ifndef TABLE_WINDOW_UI_H
#define TABLE_WINDOW_UI_H

#include "pch.h"
#include "constants.h"
#include "literal.h"

namespace Ui
{
    class TimeTableWindow
    {
    public:
        QAction *action_about;
        QAction *action_about_qt;
        QAction *action_new;
        QAction *action_open;
        QAction *action_save;
        QAction *action_save_as;
        QAction *action_quit;
        QAction *action_clear_single_item;
        QAction *action_clear_all_items;
        QAction *action_merge;
        QAction *action_unmerge;
        QAction *action_clear_and_unmerge;
        QTableView *table_data;
        QDateEdit *date_edit_begin_date;
        QLabel *label_info;
        QMenuBar *menu_bar;
        QMenu *menu_file;
        QMenu *menu_help;
        QMenu *menu_single_item;
        QMenu *menu_all_unmerged;
        QMenu *menu_all_merged;
        QMenu *menu_partially_unmerged;

        void setupUi(QMainWindow *time_table_window)
        {
            SET_OBJECT_NAME(time_table_window);
            time_table_window->resize(840, 510);
            QFont font;
            font.setPointSize(10);
            font.setStyleStrategy(QFont::PreferAntialias);
            time_table_window->setFont(font);
            time_table_window->setAcceptDrops(true);
            action_about = new QAction(STRING_LITERAL("关于"), time_table_window);
            SET_OBJECT_NAME(action_about);
            action_about_qt = new QAction(STRING_LITERAL("关于Qt"), time_table_window);
            SET_OBJECT_NAME(action_about_qt);
            action_new = new QAction(STRING_LITERAL("新建"), time_table_window);
            SET_OBJECT_NAME(action_new);
            action_open = new QAction(STRING_LITERAL("打开"), time_table_window);
            SET_OBJECT_NAME(action_open);
            action_save = new QAction(STRING_LITERAL("保存"), time_table_window);
            SET_OBJECT_NAME(action_save);
            action_save_as = new QAction(STRING_LITERAL("另存为"), time_table_window);
            SET_OBJECT_NAME(action_save_as);
            action_quit = new QAction(STRING_LITERAL("退出"), time_table_window);
            SET_OBJECT_NAME(action_quit);
            action_clear_single_item = new QAction(STRING_LITERAL("清除数据"), time_table_window);
            SET_OBJECT_NAME(action_clear_single_item);
            action_clear_all_items = new QAction(STRING_LITERAL("清除所有数据"), time_table_window);
            SET_OBJECT_NAME(action_clear_all_items);
            action_merge = new QAction(STRING_LITERAL("合并"), time_table_window);
            SET_OBJECT_NAME(action_merge);
            action_unmerge = new QAction(STRING_LITERAL("解除合并"), time_table_window);
            SET_OBJECT_NAME(action_unmerge);
            action_clear_and_unmerge = new QAction(STRING_LITERAL("清除所有数据并解除合并"), time_table_window);
            SET_OBJECT_NAME(action_clear_and_unmerge);
            QWidget *widget = new QWidget(time_table_window);
            SET_OBJECT_NAME(widget);
            QGridLayout *grid_layout = new QGridLayout(widget);
            SET_OBJECT_NAME(grid_layout);
            table_data = new QTableView(widget);
            SET_OBJECT_NAME(table_data);
            table_data->setContextMenuPolicy(Qt::CustomContextMenu);
            table_data->setSelectionMode(QAbstractItemView::ContiguousSelection);

            QHeaderView *header_view;
            header_view = table_data->horizontalHeader();
            header_view->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
            header_view->setDefaultAlignment(Qt::AlignCenter);
            header_view = table_data->verticalHeader();
            header_view->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
            header_view->setDefaultAlignment(Qt::AlignCenter);

            grid_layout->addWidget(table_data, 1, 1, 1, 3);

            QLabel *label_begin_date = new QLabel(widget);
            SET_OBJECT_NAME(label_begin_date);
            label_begin_date->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
            label_begin_date->setTextFormat(Qt::PlainText);

            grid_layout->addWidget(label_begin_date, 0, 1, 1, 1);

            date_edit_begin_date = new QDateEdit(widget);
            SET_OBJECT_NAME(date_edit_begin_date);
            date_edit_begin_date->setMaximumDate(Constants::kMaxDate);
            date_edit_begin_date->setMinimumDate(Constants::kMinDate);
            date_edit_begin_date->setCalendarPopup(true);

            grid_layout->addWidget(date_edit_begin_date, 0, 2, 1, 1);

            label_info = new QLabel(widget);
            SET_OBJECT_NAME(label_info);
            label_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
            label_info->setTextFormat(Qt::PlainText);

            grid_layout->addWidget(label_info, 0, 3, 1, 1);

            time_table_window->setCentralWidget(widget);
            menu_bar = new QMenuBar(time_table_window);
            SET_OBJECT_NAME(menu_bar);
            menu_bar->setGeometry(QRect(0, 0, 840, 23));
            menu_file = new QMenu(STRING_LITERAL("文件"), menu_bar);
            SET_OBJECT_NAME(menu_file);
            menu_help = new QMenu(STRING_LITERAL("帮助"), menu_bar);
            SET_OBJECT_NAME(menu_help);
            menu_single_item = new QMenu(menu_bar);
            SET_OBJECT_NAME(menu_single_item);
            menu_all_unmerged = new QMenu(menu_bar);
            SET_OBJECT_NAME(menu_all_unmerged);
            menu_all_merged = new QMenu(menu_bar);
            SET_OBJECT_NAME(menu_all_merged);
            menu_partially_unmerged = new QMenu(menu_bar);
            SET_OBJECT_NAME(menu_partially_unmerged);
            time_table_window->setMenuBar(menu_bar);

            menu_bar->addAction(menu_file->menuAction());
            menu_bar->addAction(menu_help->menuAction());
            menu_bar->addAction(menu_single_item->menuAction());
            menu_bar->addAction(menu_all_unmerged->menuAction());
            menu_bar->addAction(menu_all_merged->menuAction());
            menu_bar->addAction(menu_partially_unmerged->menuAction());
            menu_file->addAction(action_new);
            menu_file->addAction(action_open);
            menu_file->addAction(action_save);
            menu_file->addAction(action_save_as);
            menu_file->addAction(action_quit);
            menu_help->addAction(action_about);
            menu_help->addAction(action_about_qt);
            menu_single_item->addAction(action_clear_single_item);
            menu_all_unmerged->addAction(action_clear_all_items);
            menu_all_unmerged->addAction(action_merge);
            menu_all_merged->addAction(action_clear_single_item);
            menu_all_merged->addAction(action_unmerge);
            menu_all_merged->addAction(action_clear_and_unmerge);
            menu_partially_unmerged->addAction(action_clear_all_items);
            menu_partially_unmerged->addAction(action_merge);
            menu_partially_unmerged->addAction(action_unmerge);
            menu_partially_unmerged->addAction(action_clear_and_unmerge);

            label_begin_date->setText(STRING_LITERAL("开学日期："));
        }
    };
}

#endif // TABLE_WINDOW_UI_H
