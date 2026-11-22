# Dynamic Diagram: удаление вложенных фигур с авто-распадом (DeleteNestedCommand)

## 1. Сценарий

Пользователь выделяет одну или несколько фигур (в том числе вложенных в группы) и нажимает `Delete`.
Фигуры удаляются, а группы, в которых осталось меньше двух детей, **автоматически распадаются**
(дети поднимаются на уровень родителя). Операция **обратима**: Undo восстанавливает удалённые фигуры
и исходную структуру групп, Redo повторяет удаление и распад.

Сценарий соответствует фактическому коду:
`InputRouter/ShortcutController` → `DeleteNestedCommand` → `Picture::ExtractFigureDeep` →
`SimplifyGroups` → `CommandHistory::Execute`. Undo — через паттерн **Memento** (снимок `Picture`).

## 2. Участники (на уровне компонентов C3)

| Участник                             | Компонент C3              | Ответственность в сценарии                                                   |
|--------------------------------------|---------------------------|------------------------------------------------------------------------------|
| `ShortcutController` / `InputRouter` | Shortcuts / Input Routing | Обрабатывает `Delete`, создаёт `DeleteNestedCommand`                         |
| `DeleteNestedCommand`                | Command & Undo History    | Удаляет фигуры, каскадно упрощает группы, хранит Memento                     |
| `Picture`                            | Document Model            | `ExtractFigureDeep`, `FindParent`, `ReplaceWith`, `NotifyPictureObservers`   |
| `GroupFigure`                        | Document Model            | `ExtractChild`, `InsertChild`, `GetChildren`                                 |
| `CommandHistory`                     | Command & Undo History    | `Execute`, `Undo`, `Redo` (стек с индексом)                                  |
| `EditorState`                        | Editor State              | Сброс выделения после удаления (`ClearSelection`), восстановление после Undo |
| `ShapeListView` / `TimelineView`     | Panel Views               | Подписаны на `PictureObserver`, обновляются после `NotifyPictureObservers`   |

## 3. Последовательность (Execute → Undo → Redo)

1. **Пользователь нажимает `Delete`** (или кнопку в toolbar).
2. `ShortcutController` / `InputRouter` берёт `GetValidSelectedIds` (через `HasShapeDeep`).
3. Создаётся `DeleteNestedCommand(picture, ids)`.
4. `CommandHistory::Execute(cmd)`:
    - `cmd->Execute()`:
        - **Memento**: `ClonePicture()` — сохраняет копию `Picture` (`m_snapshot`) до удаления.
        - Для каждого id: `Picture::ExtractFigureDeep(id)` — извлекает фигуру с любой глубины.
        - `SimplifyGroups()`: цикл `while(changed)` — находит группы с < 2 детей, распаковывает их
          (`GroupFigure::ExtractChild` + `Picture::AddShape` / `parent->InsertChild`), сохраняет снимки
          распавшихся групп (`m_dissolvedGroups`).
        - `Picture::NotifyPictureObservers()` → `ShapeListView`/`TimelineView` обновляются.
    - Команда кладётся в стек, `m_commandIndex++`.
5. `EditorState::ClearSelection()` (в `ShortcutController`/`InputRouter`).
6. **Undo** (`Ctrl+Z`):
    - `CommandHistory::Undo()` → `m_commandIndex--`.
    - `cmd->Unexecute()`:
        - `ClonePictureFromSnapshot()` — создаёт `Picture` из снимка (`m_snapshot`).
        - `Picture::ReplaceWith(std::move(restored))` — заменяет внутренности.
        - `Picture::NotifyPictureObservers()` → View обновляются.
    - `ApplySelection(affectedIds)` — восстанавливает выделение (если id валидны).
7. **Redo** (`Ctrl+Y`):
    - `CommandHistory::Redo()` → `cmd->Execute()` (повторяет удаление и распад).
    - `m_commandIndex++`.

## 4. Важная деталь: Memento вместо ручного восстановления

`DeleteNestedCommand` использует **Memento** (снимок всего документа), а не ручное восстановление
путей и индексов. Это **стандартный паттерн** для Undo, когда операция сложная (каскадный распад,
вложенные группы, Z-order). Снимок делается **до** удаления (`ClonePicture`), а `Unexecute`
восстанавливает его через `ReplaceWith(ClonePictureFromSnapshot())`.

**Почему это правильно:**
- Не нужно высчитывать пути `(parentId, index)` для каждой удалённой фигуры.
- Не нужно вручную восстанавливать распавшиеся группы.
- Работает для **любой** глубины вложенности и **любого** количества удалений.
- Undo/Redo — детерминированы (снимок всегда один и тот же).

## 5. Ссылки на код

- `Commands/DeleteNestedCommand.h` — `Execute`, `Unexecute`, `SimplifyGroups`, `ClonePicture`.
- `Controller/ShortcutController.h` — `Delete` → `DeleteNestedCommand`.
- `Controller/InputRouter.h` — `HandleToolbarAction` (Delete) → `DeleteNestedCommand`.
- `Model/Picture.h` — `ExtractFigureDeep`, `FindParent`, `ReplaceWith`, `NotifyPictureObservers`.
- `Model/GroupFigure.h` — `ExtractChild`, `InsertChild`, `GetChildren`.
- `Commands/CommandHistory.h` — `Execute`, `Undo`, `Redo`.
- `Model/EditorState.h` — `ClearSelection`, `HasValidSelection`, `GetValidSelectedIds`.