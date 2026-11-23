# Архитектура графического редактора (C3 — Component)

## 1. Назначение и общая картина

Векторный графический редактор на C++20 / SFML с временной шкалой, иерархией групп,
выделением, перемещением/ресайзом (включая **вложенные фигуры внутри групп**),
палитрой заливки/обводки, **полноценным Undo/Redo**, мультидокументностью
и **зумом Canvas** (через `sf::View`, не входит в документ).

Архитектурный стиль: **MVC + Command + Observer + Visitor** с абстракцией графики
`gfx::ICanvas` (отвязка модели/контроллеров от SFML).

Диаграмма C3 (Component) показывает логические компоненты внутри контейнера
«Graphical Editor» и их взаимодействия. Диаграмма приложена отдельно
(`editor-c3.png` / `editor-c3.drawio`).

## 2. Компоненты

| #  | Компонент                          | Ответственность                                                                  | Ключевые возможности                                                                                                                                                                                                                                                             | Файлы                                                                                                                |
|----|------------------------------------|----------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------|
| 1  | **Document Model**                 | Хранение документа: фигуры, порядок, иерархия групп, время/длительность          | `AddShape`, `DeleteShapes`, `MoveShapes`, `ExtractFigure`, `ExtractFigureDeep`, `HasShapeDeep`, `GetShapeDeep`, `FindParent`, `GetTopLevelAncestor`, `HitTest`, `HitTestDeep`, `ReplaceWith`, `MoveShapeToIndex`, `ColorShapes`, `SetShapeTimes/Durations`, observer-уведомления | `Model/Document.h`, `Model/Picture.h`, `Model/Figure.h`, `Model/GroupFigure.h`, `Model/PrimitiveFigure.h`            |
| 2  | **Editor State**                   | Сессионное состояние: выделение, primary, режим, preview, currentTime, colorMode | `SetSelectedIds`, `ToggleSelected`, `ClearSelection`, `HasValidSelection` (через `HasShapeDeep`), `GetValidSelectedIds`, `IsShapeVisible` (с `GetInterval`), `preview`, `Subscribe/Unsubscribe`                                                                                  | `Model/EditorState.h`, `Model/Bounds.h`, `Model/Handle.h`                                                            |
| 3  | **Command & Undo History**         | Обратимые операции и стек undo/redo                                              | `ICommand::Execute/Unexecute/AffectedIds`, `CommandHistory::Execute/Undo/Redo/Clear`; команды Add, Delete, **DeleteNested (Memento + Simplify)**, MoveShapes, Resize, Color, SetTime, SetDuration, Reorder, Group, Ungroup                                                       | `Commands/ICommand.h`, `Commands/CommandHistory.h`, `Commands/*.h`                                                   |
| 4  | **Shape Creation**                 | Создание фигур из bounds                                                         | `Start`, `Begin`, `UpdatePreview`, `Commit` (→ `AddCommand`), `Cancel`; `MakeShape`, `ApplyResize`, `ResizeAndClamp`                                                                                                                                                             | `Controller/ShapeCreateController.h`, `Model/MakeShapeUtils.h`, `Model/ResizePolicy.h`                               |
| 5  | **Shape Transform**                | Перемещение и ресайз (включая вложенных)                                         | `TryStartResize` (через `GetShapeDeep`), `HandleShapeClick` (через `HitTestDeep`), `Update`, `Release`                                                                                                                                                                           | `Controller/ShapeTransformController.h`                                                                              |
| 6  | **Input Routing**                  | Маршрутизация SFML-событий + логика PowerPoint                                   | `HandleEvent`, `OnMousePressed` (через `HitTestDeep` + `GetTopLevelAncestor`), `HandleToolbarAction` (Delete → `DeleteNestedCommand`), `ApplyColorToSelection` (через `GetShapeDeep`)                                                                                            | `Controller/InputRouter.h`                                                                                           |
| 7  | **Shortcuts**                      | Горячие клавиши и оконные действия                                               | Ctrl+S/O/N/D, Delete (→ `DeleteNestedCommand`), Ctrl+Z/Y (→ `CommandHistory::Undo/Redo`), Ctrl+G / Ctrl+Shift+G                                                                                                                                                                  | `Controller/ShortcutController.h`, `WindowAction.h`                                                                  |
| 8  | **Document IO & File Dialog**      | New/Save/Load + нативные диалоги                                                 | `DocumentController::New/Save/Load`; `DocumentIO::Save/Load`; `SaveService`/`LoadService` (рекурсивно для групп); `FileDialog`                                                                                                                                                   | `Controller/DocumentController.h`, `IO/DocumentIO.h`, `IO/SaveService.h`, `IO/LoadService.h`, `View/FileDialog.h`    |
| 9  | **Rendering / Canvas Abstraction** | Рисование фигур и рамок (с preview для вложенных)                                | `ICanvas::DrawRectangle/...`, `SFMLCanvas`, `DrawVisitor` (**с preview-параметром**), `DrawSelectionFrame`                                                                                                                                                                       | `View/gfx/ICanvas.h`, `View/gfx/SFMLCanvas.h`, `View/DrawVisitor.h`, `View/DrawSelectionFrame.h`                     |
| 10 | **Scene View**                     | Рендер сцены, preview, рамок, toolbar, курсоров                                  | `Render` (через `GetShapeDeep` для рамки), `DrawPreview`, `UpdateCursor` (через `GetShapeDeep`)                                                                                                                                                                                  | `View/EditorView.h`, `View/Toolbar.h`, `View/CursorType.h`                                                           |
| 11 | **Panel Views**                    | Список фигур и таймлайн (с группами)                                             | `ShapeListView::Render/HandleClick`; `TimelineView::Render/HandleClick/UpdatePreview/Release/Zoom`, `GroupInterval` (охватывающий интервал группы)                                                                                                                               | `View/ShapeListView.h`, `View/TimelineView.h`                                                                        |
| 12 | **Window / Composition Root**      | Окно, композиция, мультидокументность, **зум/камера**                            | `EditorWindowController` (создаёт View/Controller/State, `m_camera` — `sf::View` для зума и viewport), `EditorController`, `DocumentRegistry`, `WindowAction`, `ZoomAt`, `ResetZoom`                                                                                             | `Controller/EditorWindowController.h`, `Controller/EditorController.h`, `Model/DocumentRegistry.h`, `WindowAction.h` |

**Почему именно 12 компонентов:** выделение идёт по реальным границам ответственности
(Model / Controller / View / Command / IO), а не по каждому файлу.
`Panel Views` объединены, т.к. `ShapeListView` и `TimelineView` однотипны по роли.
`Observer Infrastructure` (`ObserverList`, `PictureObserver`, `EditorStateObserver`) — это
**механизм**, а не компонент, поэтому показан связями «observer», а не отдельным прямоугольником.

## 3. Как обрабатывается пользовательское действие

Пример: пользователь кликает по ребёнку внутри группы (поведение PowerPoint).

1. `EditorWindowController::HandleEvents` → `EditorController::HandleEvent` → `InputRouter::HandleEvent`.
2. `InputRouter::OnMousePressed`:
    - `HitTestDeep(x, y)` → самый глубокий id.
    - Если id **вложенный**:
        - `GetTopLevelAncestor(id)` → группа верхнего уровня.
        - Если группа **выделена** → `SetSelectedIds({ребёнок})` и `mode = Dragging`.
        - Иначе → `SetSelectedIds({группа})` и `mode = Dragging`.
    - Если id **верхний уровень** → `HandleShapeClick` (обычное выделение).
3. `InputRouter::OnMouseMoved` → `ShapeTransformController::Update` → `Drag`:
    - считает `dx/dy` с клампом;
    - **пишет preview в `EditorState::preview[id].bounds`** (документ не меняется).
4. `EditorView::Render` → `DrawVisitor` (с `preview`) → для группы обходит детей и применяет preview к каждому ребёнку.
5. `InputRouter::OnMouseReleased` → `ShapeTransformController::Release`:
    - `MoveShapesCommand` (через `GetShapeDeep`) → `CommandHistory::Execute`.
    - `ClearPreview()`, `mode = Idle`.

**Ключевая идея:** документ меняется только через `CommandHistory::Execute`.
Preview — в `EditorState`.

## 4. Как изменяется документ

- `Document` хранит `Picture` и `CommandHistory`.
- Любое изменение `Picture` (`AddShape`, `DeleteShapes`, `MoveShapes`, `SetShapeBounds`,
  `ColorShapes`, `SetShapeTimes`, `SetShapeDurations`, `MoveShapeToIndex`, `ExtractFigure`,
  `ExtractFigureDeep`, `ReplaceWith`) вызывает `NotifyPictureObservers()`.
- `ShapeListView` и `TimelineView` подписаны на `PictureObserver` и обновляют кэш.
- `EditorState` уведомляет `EditorStateObserver` при изменении выделения.

## 5. Где обеспечивается Undo/Redo

Централизованно в `CommandHistory` (внутри `Document`, не глобально).

- `Execute`: применяет, отрезает redo-хвост, кладёт в стек, `m_commandIndex++`.
- `Undo`: `m_commandIndex--`, `Unexecute()`, возвращает `AffectedIds()`.
- `Redo`: `Execute()`, `m_commandIndex++`.
- `Clear()` при New/Load.

Каждая команда хранит всё необходимое для отмены:

| Команда                   | Что хранит для Undo                                                                                                     |
|---------------------------|-------------------------------------------------------------------------------------------------------------------------|
| `AddCommand`              | `unique_ptr<Figure>` + id                                                                                               |
| `DeleteCommand`           | `(index, unique_ptr<Figure>)` для верхнего уровня                                                                       |
| **`DeleteNestedCommand`** | **Memento: полная копия `Picture` (`m_snapshot`) + список id. `Unexecute` → `ReplaceWith(ClonePictureFromSnapshot())`** |
| `MoveShapesCommand`       | `dx/dy`                                                                                                                 |
| `ResizeCommand`           | `oldBounds/newBounds`                                                                                                   |
| `ColorCommand`            | `oldColors` для каждого id (рекурсивно для групп)                                                                       |
| `SetTimeCommand`          | `oldTimes`, флаг `alreadyApplied`                                                                                       |
| `SetDurationCommand`      | `oldDurations`                                                                                                          |
| `ReorderCommand`          | `oldIdx/newIdx`                                                                                                         |
| `GroupCommand`            | `(id, index)` детей + id группы                                                                                         |
| `UngroupCommand`          | снимок группы (цвет, time, duration) + id детей + `m_groupIndex`                                                        |

**Восстановление выделения** после Undo/Redo: `ShortcutController::ApplySelection` фильтрует
`AffectedIds()` по `Picture::HasShapeDeep` и ставит в `EditorState`.

**Почему `DeleteNestedCommand` использует Memento:** ручное восстановление путей и индексов при
каскадном распаде групп — сложно и подвержено ошибкам. Снимок документа (`Picture::Clone`) —
стандартный паттерн для Undo, надёжный и простой.

## 6. As-is vs желаемое (честно)

Реализовано хорошо:

- Command Pattern с undo/redo;
- Observer для View (развязаны от модели);
- Visitor для отрисовки (`DrawVisitor` с preview);
- `ICanvas` — абстракция над SFML;
- рекурсивная сериализация групп (`SaveService`/`LoadService`);
- `Picture` умеет глубокий поиск (`HasShapeDeep`, `GetShapeDeep`, `HitTestDeep`);
- `DrawVisitor` применяет preview к вложенным;
- `sf::View` для зума и viewport (fullscreen без растягивания);
- `DeleteNestedCommand` с Memento.

Размытые границы (не выдаём желаемое за реализованное):

1. **`Picture` остаётся плоским для верхнего уровня**, но умеет глубокий поиск
   (компромисс: `m_shapes` — только верхний уровень, вложенные — через рекурсию).
2. **`InputRouter` — «толстый»**: маршрутизация + создание команд + логика PowerPoint.
3. **`TimelineView` — «толстый»**: рендер + drag-состояние + `TimelineDragResult` + zoom.
4. **`EditorView` — композитный**: сцена + preview + рамки + toolbar + курсоры.
5. **`EditorState` совмещает** выделение и transient-состояние операций (drag/resize/preview/create).
6. **Создание команд распределено** между `InputRouter`, `ShortcutController`,
   `ShapeCreateController`, `ShapeTransformController`.
7. **`sf::View` в `EditorWindowController`** — это View-уровень, но зум **не** входит в документ,
   Undo/Redo и Save/Load. Каждое окно имеет свой `m_camera`.
