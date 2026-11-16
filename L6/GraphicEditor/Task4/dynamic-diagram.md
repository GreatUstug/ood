# Dynamic Diagram: перемещение фигуры по таймлайну (Time Drag)

## 1. Сценарий

Пользователь перетаскивает фигуру по горизонтали в `TimelineView`. Время фигуры
(`time`) меняется. Операция **обратима**: Undo возвращает старое время, Redo
повторяет новое.

Сценарий соответствует фактическому коду:
`TimelineView::HandleClick` → `UpdatePreview` → `Release` →
`InputRouter::CreateTimelineCommand` → `CommandHistory::Execute(SetTimeCommand)`.

## 2. Участники (на уровне компонентов C3)

| Участник             | Компонент C3           | Ответственность в сценарии                                          |
|----------------------|------------------------|---------------------------------------------------------------------|
| `TimelineView`       | Panel Views            | hit-test, drag-состояние, preview, `Release` → `TimelineDragResult` |
| `InputRouter`        | Input Routing          | маршрутизация событий, создание `SetTimeCommand`                    |
| `EditorState`        | Editor State           | хранение `preview[id].time` (временное состояние до commit)         |
| `EditorView`         | Scene View             | рендер preview по `EditorState::preview`                            |
| `CommandHistory`     | Command & Undo History | `Execute`, стек undo/redo                                           |
| `SetTimeCommand`     | Command & Undo History | `Execute`/`Unexecute`, хранит `oldTimes`, флаг `alreadyApplied`     |
| `Picture`            | Document Model         | `SetShapeTimes`, `NotifyPictureObservers`                           |
| `ShortcutController` | Shortcuts              | Ctrl+Z / Ctrl+Y → `Undo`/`Redo`                                     |

## 3. Последовательность (preview → commit → undo)

1. **MousePressed** в таймлайне:
    - `InputRouter::OnMousePressed` → `TimelineView::HandleClick`;
    - hit-test по бару фигуры, `m_pendingDrag = true`, `m_dragMode = None`,
      `m_dragStartShapeTime = t`, `m_dragStartMouseTime = clickTime`.
2. **MouseMoved** (drag):
    - `InputRouter::OnMouseMoved` → `TimelineView::UpdatePreview`;
    - после dead-zone `m_dragMode = Time`;
    - `newTime = m_dragStartShapeTime + (clickTime - m_dragStartMouseTime)`;
    - **`EditorState::preview[id].time = newTime`** (preview, документ не меняется).
3. **Рендер** (каждый кадр):
    - `EditorView::Render` читает `m_state.preview` и рисует preview-версию бара;
    - `TimelineView::Render` тоже читает `preview[id].time` для отрисовки.
4. **MouseReleased** (commit-намерение):
    - `InputRouter::OnMouseReleased` → `TimelineView::Release()`;
    - `Release` проверяет, что время изменилось, **удаляет preview**
      (`m_state.preview.erase(m_dragId)`) и возвращает `TimelineDragResult{Time, oldTime, newTime}`.
5. **Создание команды**:
    - `InputRouter::CreateTimelineCommand(result)` → `SetTimeCommand(picture, {id}, {newTime}, false)`.
6. **Commit**:
    - `Document::GetHistory().Execute(cmd)`:
        - `SetTimeCommand::Execute` → `Picture::SetShapeTimes(ids, newTimes)`;
        - `Picture::NotifyPictureObservers()` → панели обновляют `ShapeInfo`;
        - команда кладётся в стек, `m_commandIndex++`.
7. **Undo** (Ctrl+Z):
    - `ShortcutController` → `CommandHistory::Undo()`;
    - `SetTimeCommand::Unexecute` → `Picture::SetShapeTimes(ids, oldTimes)`;
    - `ShortcutController::ApplySelection(affectedIds)` → выделение восстанавливается.
8. **Redo** (Ctrl+Y):
    - `CommandHistory::Redo()` → `SetTimeCommand::Execute` → `SetShapeTimes(ids, newTimes)`.

## 4. Важная деталь: preview vs commit

- **Preview** - это `EditorState::preview[id].time`. Документ **не меняется**,
  наблюдатели `PictureObserver` **не уведомляются**. `EditorView`/`TimelineView`
  читают preview при рендере.
- **Commit** - только в `CommandHistory::Execute(SetTimeCommand)`. С этого момента
  изменение в документе и в истории.
- **`SetTimeCommand::alreadyApplied`** - флаг для случая, когда значение уже
  записано в preview, и команда не должна применять его повторно при первом
  `Execute`. В текущем сценарии `InputRouter::CreateTimelineCommand` передаёт
  `false`, потому что `Release` уже удалил preview и значение ещё не в документе.
  Флаг - осознанный костыль под сценарий preview→commit, его надо честно
  упомянуть.

## 5. Ссылки на код

- `View/TimelineView.h` - `HandleClick`, `UpdatePreview`, `Release`, `DragMode`.
- `Controller/InputRouter.h` - `OnMousePressed`, `OnMouseMoved`, `OnMouseReleased`,
  `CreateTimelineCommand`.
- `Model/EditorState.h` - `preview`, `PreviewShape`.
- `Commands/SetTimeCommand.h` - `Execute`, `Unexecute`, `alreadyApplied`.
- `Commands/CommandHistory.h` - `Execute`, `Undo`, `Redo`.
- `Controller/ShortcutController.h` - Ctrl+Z / Ctrl+Y, `ApplySelection`.