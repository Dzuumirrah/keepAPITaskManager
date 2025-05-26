#ifndef TASK_TYPES_H
#define TASK_TYPES_H

#include <Arduino.h>
#include <vector>

struct RawTask {
  String list_id;
  String list_title;
  String task_id;
  String parentId;
  int position;
  String title;
  bool completed;
  String due;
  String notes;        // ← new field for your description
};

// No change to Task’s pointers/children, just add `notes`:
struct Task {
  String id;
  String title;
  bool completed;
  String due;
  String notes;        // ← carry through the description
  int position;
  String parentId;
  std::vector<Task*> children;
};

#endif  // TASK_TYPES_H