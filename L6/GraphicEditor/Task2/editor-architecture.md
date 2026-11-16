# Архитектура графического редактора (C3 — Component)

## 1. Назначение и общая картина

Векторный графический редактор на C++/SFML с временной шкалой, иерархией групп,
выделением, перемещением/ресайзом, палитрой заливки/обводки, полноценным Undo/Redo
и мультидокументностью (несколько окон).

Архитектурный стиль: **MVC + Command + Observer + Visitor** с абстракцией графики
(`gfx::ICanvas`), что отвязывает модель и контроллеры от SFML.

Диаграмма C3 (Component) показывает логические компоненты внутри одного контейнера
«Graphical Editor» и их взаимодействия. Диаграмма приложена отдельно
(`editor-c3.drawio` / `editor-c3.png`).

## 2. Компоненты

| # | Компонент | Ответственность | Ключевые возможности | Файлы |
|---|-----------|-----------------|----------------------|-------|
| 1 | **Document Model** | Хранение документа: фигуры верхнего уровня, порядок отрисовки, иерархия групп, время/длительность | `AddShape`, `DeleteShapes`, `ExtractFigure`, `MoveShapes`, `SetShapeBounds`, `ColorShapes`, `SetShapeTimes`, `SetShapeDurations`, `MoveShapeToIndex`, `HitTest`, `ReplaceWith`, `GetOrder`, `GetShape`, `NotifyPictureObservers` | `Model/Document.h`, `Model/Picture.h`, `Model/Figure.h`, `Model/GroupFigure.h`, `Model/PrimitiveFigure.h`, `Model/ShapeInfo.h` |
| 2 | **Editor State** | Сессионное состояние: выделение, primary, режим, preview, текущее время, режим цвета | `SetSelectedIds`, `SetPrimarySelectedId`, `ToggleSelected`, `ClearSelection`, `HasValidSelection`, `GetValidSelectedIds`, `IsShapeVisible`, `preview`, `CancelCreating`, `ClearPreview`, `Subscribe/Unsubscribe` | `Model/EditorState.h`, `Model/Bounds.h`, `Model/Handle.h` |
| 3 | **Command & Undo History** | Обратимые операции и стек undo/redo | `ICommand::Execute/Unexecute/AffectedIds`; `CommandHistory::Execute/Undo/Redo/Clear`; команды `Add`, `Delete`, `MoveShapes`, `Resize`, `Color`, `SetTime`, `SetDuration`, `Reorder`, `Group`, `Ungroup` | `Commands/ICommand.h`, `Commands/CommandHistory.h`, `Commands/*.h` |
| 4 | **Shape Creation** | Создание фигур из bounds | `Start(type)`, `Begin(pos)`, `UpdatePreview(pos)`, `Commit()` → `AddCommand`, `Cancel()`; фабрика `MakeShape`; `ApplyResize`, `ResizeAndClamp` | `Controller/ShapeCreateController.h`, `Model/MakeShapeUtils.h`, `Model/ResizePolicy.h` |
| 5 | **Shape Transform** | Перемещение и ресайз выделения | `TryStartResize(pos)`, `HandleShapeClick(pos)`, `Update(pos)`, `Release()` → `MoveShapesCommand`/`ResizeCommand`; работа с preview | `Controller/ShapeTransformController.h` |
| 6 | **Input Routing** | Маршрутизация SFML-событий по View и контроллерам | `HandleEvent(event)`, `OnMousePressed/Moved/Released`, `HandleToolbarClick`, `ApplyColorToSelection`, `CreateTimelineCommand` | `Controller/InputRouter.h` |
| 7 | **Shortcuts** | Горячие клавиши и оконные действия | Ctrl+S/O/N/D, Delete, Ctrl+Z/Y, Ctrl+G / Ctrl+Shift+G; возвращает `WindowAction` | `Controller/ShortcutController.h`, `WindowAction.h` |
| 8 | **Document IO & File Dialog** | New/Save/Load и нативные диалоги | `DocumentController::New/Save/Load`; `DocumentIO::Save/Load`; `SaveService`/`LoadService` (рекурсивно для групп); `FileDialog::AskSavePath/AskOpenPath/ShowError` | `Controller/DocumentController.h`, `IO/DocumentIO.h`, `IO/SaveService.h`, `IO/LoadService.h`, `View/FileDialog.h` |
| 9 | **Rendering / Canvas Abstraction** | Рисование фигур и рамок через абстрактный canvas | `ICanvas::DrawRectangle/Ellipse/Triangle/Line/Polygon`, `SFMLCanvas`, `DrawVisitor` (Visitor), `DrawSelectionFrame`, `GetActualHitTestHandle`, `HandlesOf` | `View/gfx/ICanvas.h`, `View/gfx/SFMLCanvas.h`, `View/DrawVisitor.h`, `View/DrawSelectionFrame.h` |
| 10 | **Scene View** | Рендер сцены, preview создания, рамок, toolbar, курсоров | `Render`, `DrawPreview`, `UpdateCursor`, `RenderToolbar` | `View/EditorView.h`, `View/Toolbar.h`, `View/CursorType.h` |
| 11 | **Panel Views** | Список фигур и таймлайн | `ShapeListView::Render/HandleClick` (подписан на Picture + EditorState); `TimelineView::Render/HandleClick/UpdatePreview/Release/Zoom` | `View/ShapeListView.h`, `View/TimelineView.h` |
| 12 | **Window / Composition Root** | Окно, композиция, мультидокументность | `EditorWindowController` (создаёт View/Controller/State), `EditorController` (фасад, `HandleEvent`/`TakePendingCommand`), `DocumentRegistry::Create/CreateFrom/Prune`, `WindowAction` | `Controller/EditorWindowController.h`, `Controller/EditorController.h`, `Model/DocumentRegistry.h`, `WindowAction.h` |

**Почему именно 12.** Выделение идёт по реальным границам ответственности
(Model / Controller / View / Command / IO), а не по каждому файлу.
`Panel Views` объединены, т.к. `ShapeListView` и `TimelineView` однотипны по роли
(панель, подписанная на модель и состояние). `Observer Infrastructure`
(`ObserverList`, `PictureObserver`, `EditorStateObserver`) — это **механизм**, а не
компонент, поэтому показан связями «observer», а не отдельным прямоугольником.

## 3. Как обрабатывается пользовательское действие

Пример: пользователь тянет фигуру мышью по сцене.

1. `EditorWindowController::HandleEvents` → `pollEvent` →
   `EditorController::HandleEvent` → `InputRouter::HandleEvent`.
2. `InputRouter::OnMousePressed`:
    - попали в `TimelineView` → `TimelineView::HandleClick`;
    - попали в `ShapeListView` → `ShapeListView::HandleClick`;
    - попали в `Toolbar` → `HandleToolbarAction` / `ApplyColorToSelection`;
    - иначе → `ShapeTransformController::TryStartResize` или `HandleShapeClick`.
3. `ShapeTransformController::HandleShapeClick`:
    - `Picture::HitTest` → id верхнеуровневой фигуры (для группы — сама группа);
    - обновляет `EditorState` (выделение, `dragAnchor`, `dragStartBounds`);
    - `mode = Dragging`.
4. `InputRouter::OnMouseMoved` → `ShapeTransformController::Update` → `Drag`:
    - считает `dx/dy` с клампом по канвасу;
    - **пишет preview в `EditorState::preview[id].bounds`** (документ не меняется).
5. `EditorView::Render` читает `EditorState::preview` и рисует preview-версию
   фигуры (`Clone` + `SetBounds`).
6. `InputRouter::OnMouseReleased` → `ShapeTransformController::Release`:
    - если `dx/dy != 0` → `Document::GetHistory().Execute(MoveShapesCommand(...))`;
    - `ClearPreview()`, `mode = Idle`.

**Ключевая идея:** документ меняется только через `CommandHistory::Execute`.
Preview — временное состояние в `EditorState`, не команда.

## 4. Как изменяется документ

- `Document` хранит `shapes::Picture` и `CommandHistory`.
- Любое изменение `Picture` (`AddShape`, `DeleteShapes`, `MoveShapes`,
  `SetShapeBounds`, `ColorShapes`, `SetShapeTimes`, `SetShapeDurations`,
  `MoveShapeToIndex`, `ExtractFigure`, `ReplaceWith`) вызывает
  `NotifyPictureObservers()` → `OnPictureChanged(BuildShapeInfo())`.
- `ShapeListView` и `TimelineView` подписаны на `PictureObserver` и кэшируют
  `ShapeInfo` для рендера.
- `EditorState` уведомляет `EditorStateObserver` при изменении выделения.

## 5. Где обеспечивается Undo/Redo

Централизованно в `CommandHistory`, который живёт **внутри `Document`**
(не глобально — это важно для мультидокументности).

```cpp
void Execute(std::unique_ptr<ICommand> command) {
    command->Execute();                      // применить
    if (m_commandIndex < m_commands.size())  // отрезать redo-хвост
        m_commands.erase(m_commands.begin() + m_commandIndex, m_commands.end());
    m_commands.push_back(std::move(command));
    m_commandIndex++;
}