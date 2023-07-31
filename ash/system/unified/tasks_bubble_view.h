// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ASH_SYSTEM_UNIFIED_TASKS_BUBBLE_VIEW_H_
#define ASH_SYSTEM_UNIFIED_TASKS_BUBBLE_VIEW_H_

#include "ash/ash_export.h"
#include "ash/glanceables/tasks/glanceables_tasks_types.h"
#include "ash/system/unified/glanceable_tray_child_bubble.h"
#include "base/memory/raw_ptr.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/base/models/list_model.h"
#include "ui/views/layout/flex_layout_view.h"

namespace views {
class Combobox;
class ImageView;
}  // namespace views

namespace ash {

class GlanceablesListFooterView;
class TasksComboboxModel;

// 'TasksBubbleView' uses nested `FlexLayoutView`s to layout the tasks bubble.
// configurations.
// +---------------------------------------------------------------+
// |`TasksBubbleView`                                              |
// | +-----------------------------------------------------------+ |
// | |'tasks_header_view_'                                       | |
// | +-----------------------------------------------------------+ |
// | +-----------------------------------------------------------+ |
// | |'task_items_container_view_'                               | |
// | +-----------------------------------------------------------+ |
// | +-----------------------------------------------------------+ |
// | |'tasks_footer_view_'                                       | |
// | +-----------------------------------------------------------+ |
// +---------------------------------------------------------------+
//
// +----------------------------------------------+
// |`tasks_header_view_`                          |
// |+---------------+ +-------------------------+ |
// ||task_icon_view_| |task_list_combo_box_view_| |
// |+---------------+ +-------------------------+ |
// +----------------------------------------------+
//
// +----------------------------------------------------------------+
// |'task_items_container_view_'                                    |
// | +------------------------------------------------------------+ |
// | |GlanceablesTaskView                                         | |
// | +----------------------------------------------------------- + |
// | +----------------------------------------------------------- + |
// | |GlanceablesTaskView                                         | |
// | +----------------------------------------------------------- + |
// +----------------------------------------------------------------+
//
// +--------------------------------------------------------------+
// |'list_footer_view_'                                           |
// +--------------------------------------------------------------+

class ASH_EXPORT TasksBubbleView : public GlanceableTrayChildBubble {
 public:
  METADATA_HEADER(TasksBubbleView);

  explicit TasksBubbleView(DetailedViewDelegate* delegate);
  TasksBubbleView(const TasksBubbleView&) = delete;
  TasksBubbleView& operator=(const TasksBubbleView&) = delete;
  ~TasksBubbleView() override;

  bool IsMenuRunning();

  views::Combobox* task_list_combo_box_view() const {
    return task_list_combo_box_view_;
  }

  views::View* task_items_container_view() const {
    return task_items_container_view_;
  }

  // views::View:
  void GetAccessibleNodeData(ui::AXNodeData* node_data) override;

 private:
  friend class DateTrayTest;
  // Setup child views.
  void InitViews(ui::ListModel<GlanceablesTaskList>* task_list);

  // Handles on-click behavior for the "See all" button in `list_footer_view_`.
  void ActionButtonPressed();

  // Handles switching between tasks lists.
  void SelectedTasksListChanged();
  void ScheduleUpdateTasksList();
  void UpdateTasksList(const std::string& task_list_id,
                       ui::ListModel<GlanceablesTask>* tasks);

  // Model for the combobox used to change the active task list.
  std::unique_ptr<TasksComboboxModel> tasks_combobox_model_;

  // Tracks the number of tasks show. Used for sizing.
  int num_tasks_shown_ = 0;

  // Owned by views hierarchy.
  raw_ptr<views::FlexLayoutView, ExperimentalAsh> tasks_header_view_ = nullptr;
  raw_ptr<views::ImageView, ExperimentalAsh> task_icon_view_ = nullptr;
  raw_ptr<views::Combobox, ExperimentalAsh> task_list_combo_box_view_ = nullptr;
  raw_ptr<views::FlexLayoutView, ExperimentalAsh> button_container_ = nullptr;
  raw_ptr<views::View, ExperimentalAsh> task_items_container_view_ = nullptr;
  raw_ptr<GlanceablesListFooterView, ExperimentalAsh> list_footer_view_ =
      nullptr;

  base::WeakPtrFactory<TasksBubbleView> weak_ptr_factory_{this};
};

}  // namespace ash

#endif  // ASH_SYSTEM_UNIFIED_TASKS_BUBBLE_VIEW_H_
