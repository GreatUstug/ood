# AGENTS.md — контекст для AI-агента: графический редактор

## 1. Назначение проекта

Векторный графический редактор на C++20 / SFML с временной шкалой, иерархией групп,
выделением, перемещением/ресайзом (включая вложенные фигуры), палитрой заливки/обводки,
полноценным Undo/Redo, мультидокументностью и зумом Canvas (через `sf::View`).

Архитектурный стиль: MVC + Command + Observer + Visitor с абстракцией графики `gfx::ICanvas`.

## 2. Слои и ключевой код

| Слой             | Где                                                                                                                                                                                                                                         | Что внутри                                                                                                                   |
|------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------|
| Composition Root | `Controller/EditorWindowController.h`, `Controller/EditorController.h`, `Model/DocumentRegistry.h`, `WindowAction.h`                                                                                                                        | Создание View/Controller/State, `sf::View m_camera` (зум), рендер-цикл, мультидокументность                                  |
| Controllers      | `Controller/InputRouter.h`, `Controller/ShortcutController.h`, `Controller/ShapeCreateController.h`, `Controller/ShapeTransformController.h`, `Controller/DocumentController.h`                                                             | Маршрутизация событий, логика PowerPoint (HitTestDeep), создание/трансформация, IO                                           |
| Model            | `Model/Document.h`, `Model/Picture.h`, `Model/Figure.h`, `Model/GroupFigure.h`, `Model/PrimitiveFigure.h`, `Model/EditorState.h`, `Model/Bounds.h`, `Model/Handle.h`, `Model/ResizePolicy.h`, `Model/MakeShapeUtils.h`, `Model/ShapeInfo.h` | Документ, фигуры, иерархия групп, выделение, preview, bounds                                                                 |
| Commands         | `Commands/ICommand.h`, `Commands/CommandHistory.h`, `Commands/*.h`                                                                                                                                                                          | Add, Delete, **DeleteNested (Memento + Simplify)**, MoveShapes, Resize, Color, SetTime, SetDuration, Reorder, Group, Ungroup |
| Views            | `View/EditorView.h`, `View/ShapeListView.h`, `View/TimelineView.h`, `View/Toolbar.h`, `View/DrawVisitor.h`, `View/DrawSelectionFrame.h`, `View/gfx/ICanvas.h`, `View/gfx/SFMLCanvas.h`                                                      | Рендер сцены, панелей, таймлайна, Visitor (с preview), абстракция canvas                                                     |
| IO               | `IO/DocumentIO.h`, `IO/SaveService.h`, `IO/LoadService.h`, `View/FileDialog.h`                                                                                                                                                              | Текстовый формат, рекурсивная сериализация групп, нативные диалоги                                                           |

## 3. Ключевые архитектурные факты

### 3.1. `Picture` — плоский верхний уровень + глубокий поиск
- `Picture` хранит `unordered_map<string, unique_ptr<Figure>> m_shapes` + `vector<string> m_order` (верхний уровень).
- `GroupFigure` хранит `vector<unique_ptr<Figure>> m_children` — отдельное дерево.
- **Глубокий API**: `HasShapeDeep`, `GetShapeDeep`, `HitTestDeep`, `FindParent`, `GetTopLevelAncestor`, `ExtractFigureDeep`.
- `MoveShapes`, `SetShapeBounds`, `ColorShapes`, `SetShapeTimes/Durations` работают через `GetShapeDeep` (поддерживают вложенные).

### 3.2. Документ меняется только через команды
- Все изменения `Picture` — через `CommandHistory::Execute(cmd)`.
- `Picture::NotifyPictureObservers()` вызывается после каждого изменения.

### 3.3. Preview в `EditorState`, commit — команда
- `EditorState::preview` — `unordered_map<string, PreviewShape>` (bounds, time, duration).
- `DrawVisitor` принимает `preview` и применяет его к вложенным детям при отрисовке группы.
- Commit — только `CommandHistory::Execute`.

### 3.4. Undo/Redo — `CommandHistory` внутри `Document`
- `Execute`: применяет, отрезает redo-хвост, кладёт в стек.
- `Undo`: `Unexecute()`, возвращает `AffectedIds()`.
- `Redo`: `Execute()`, `m_commandIndex++`.
- **`DeleteNestedCommand` использует Memento** (снимок `Picture`) вместо ручного восстановления путей.

### 3.5. «Толстые» классы (as-is)
- `InputRouter` — маршрутизация + логика PowerPoint + создание команд.
- `TimelineView` — рендер + drag + `TimelineDragResult` + zoom.
- `EditorView` — сцена + preview + рамки + toolbar + курсоры.
- `EditorState` — выделение + transient-состояние операций.

### 3.6. Зум — `sf::View` в `EditorWindowController`
- `m_camera` — `sf::View` для зума и viewport.
- Зум **не** входит в документ, Undo/Redo, Save/Load.
- Каждое окно имеет свой `m_camera`.

## 4. Ограничения и правила для изменений

- **Не менять `Picture` на дерево без анализа** — это затронет View, EditorState, HitTest, Delete.
- **Не добавлять команды в обход `CommandHistory`** — иначе Undo/Redo развалится.
- **Не писать в `Picture` напрямую из View** — только через контроллеры/команды.
- **Не дублировать точку создания `DeleteCommand`** — сейчас их две (`InputRouter`, `ShortcutController`).
- **Preview — только в `EditorState`**, commit — только команда.
- **`sf::View` — только в `EditorWindowController`**, не в модели.

## 5. Точки входа

- `main` → `DocumentRegistry` → `EditorWindowController` → `EditorController` → `InputRouter`.
- Рендер: `EditorWindowController::Render` → `setView(m_camera)` → `EditorView`, `ShapeListView`, `TimelineView`.
- События: `EditorWindowController::HandleEvents` → `EditorController::HandleEvent` → `InputRouter::HandleEvent`.
- Undo/Redo: `ShortcutController` (Ctrl+Z/Y) → `CommandHistory::Undo/Redo`.

## 6. Задача для AI-агента

См. `ai-request.md`. Нужен **план изменений без редактирования кода**.