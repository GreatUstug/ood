# Анализ влияния нового требования: добавление слоёв (Layers)

## 1. Новое требование

Пользователь должен иметь возможность создавать **слои** (Layers), управлять их видимостью
и перемещать фигуры между слоями. Перемещение фигуры между слоями должно быть **обратимым**:
Undo возвращает фигуру на прежний слой и восстанавливает её Z-order, Redo повторяет перемещение.
Слои не являются частью фигуры; это отдельная сущность документа, которая группирует фигуры
по уровням отображения.

Реализовывать не требуется — только архитектурный анализ.

## 2. Исходное состояние (as-is)

По C3-диаграмме (`editor-c3`) и коду редактора:

- `Picture` хранит **плоский** `m_order` верхнего уровня и `m_shapes` (id → `unique_ptr<Figure>`).
  Иерархия групп реализована внутри `GroupFigure::m_children`, но `Picture` не знает о слоях.
- Z-order определяется порядком в `m_order` (для верхнего уровня) и в `GroupFigure::m_children`.
- `EditorState` хранит `m_selectedIds` (`set<string>`) и `preview` (временное состояние операций).
- `CommandHistory` хранит команды (`Add`, `DeleteNested`, `MoveShapes`, `Resize`, `Color`, `SetTime`,
  `SetDuration`, `Reorder`, `Group`, `Ungroup`). Все команды работают с `Picture`.
- `ShapeListView` и `TimelineView` подписаны на `PictureObserver` и показывают **плоский** список
  верхнего уровня (без слоёв).
- `DrawVisitor` рекурсивно обходит группы и рисует детей с учётом `preview`.
- `EditorWindowController` владеет `sf::View` (`m_camera`) для зума и viewport; это **не** часть документа.

**Вывод as-is:** в текущей архитектуре **нет** понятия «слой». Z-order — это просто порядок фигур
в `m_order`. Группы — это вложенные фигуры, а не слои.

## 3. Затронутые компоненты (таблица)

| Компонент                     | Что меняется                                                                                                                                                                                                                     | Причина                                                                                 | Файлы                                                                |
|-------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------|----------------------------------------------------------------------|
| **Document Model**            | Новый класс `Layer` (id, name, visible, order). `Picture` хранит `vector<Layer>` и для каждой фигуры — id слоя. `AddShape`/`ExtractFigureDeep`/`MoveShapeToIndex` учитывают слой. `GetOrder` возвращает порядок **внутри** слоя. | Слой — новая сущность документа. Без неё нельзя группировать фигуры по уровням.         | `Model/Layer.h` (новый), `Model/Picture.h`, `Model/Document.h`       |
| **Editor State**              | Новое поле `activeLayerId`. Выделение (`m_selectedIds`) должно учитывать, что фигура принадлежит слою. `IsShapeVisible` учитывает видимость слоя (скрытые слои не рисуются).                                                     | Пользователь работает с **активным** слоем. Скрытые слои не должны попадать в hit-test. | `Model/EditorState.h`                                                |
| **Command & Undo History**    | Новая команда `MoveToLayerCommand` (или `SetLayerCommand`): сохраняет id фигуры, старый слой, новый слой, старый индекс. `Unexecute` возвращает фигуру на прежний слой и позицию.                                                | Перемещение между слоями — обратимая операция. Нужна команда.                           | `Commands/MoveToLayerCommand.h` (новый), `Commands/CommandHistory.h` |
| **Input Routing**             | `OnMousePressed` должен учитывать видимость слоя (скрытые слои не hit-test'ятся). `HandleToolbarAction` — новые действия «Создать слой», «Скрыть/Показать», «Переместить в слой».                                                | Пользователь управляет слоями через UI.                                                 | `Controller/InputRouter.h`                                           |
| **Shortcuts**                 | Новые горячие клавиши: `Ctrl+L` (создать слой), `Ctrl+H` (скрыть/показать), `Ctrl+Shift+L` (переместить в слой).                                                                                                                 | Управление слоями с клавиатуры.                                                         | `Controller/ShortcutController.h`                                    |
| **Panel Views**               | `ShapeListView` показывает **дерево слоёв** (сверху — слои, внутри — фигуры). `TimelineView` — полоски фигур с учётом слоя (опционально).                                                                                        | Пользователь должен видеть, к какому слою относится фигура.                             | `View/ShapeListView.h`, `Model/ShapeInfo.h` (добавить `layerId`)     |
| **Scene View / Rendering**    | `DrawVisitor` (или `EditorView::Render`) должен **пропускать** фигуры из скрытых слоёв. `DrawSelectionFrame` — рисовать только для видимых.                                                                                      | Скрытые слои не должны отображаться.                                                    | `View/EditorView.h`, `View/DrawVisitor.h`                            |
| **Document IO**               | Сериализация слоёв: `SaveService`/`LoadService` должны сохранять `Layer` (id, name, visible) и `layerId` для каждой фигуры. Формат — обратно совместимый (документы без слоёв загружаются как «слой по умолчанию»).              | Слои — часть документа, их надо сохранять.                                              | `IO/SaveService.h`, `IO/LoadService.h`                               |
| **Window / Composition Root** | UI для управления слоями (панель слоёв). Можно добавить в `Toolbar` или отдельную панель.                                                                                                                                        | Пользователю нужен интерфейс для слоёв.                                                 | `View/Toolbar.h` (или новая панель)                                  |

## 4. Сценарий (to-be): от действия до Undo

1. Пользователь нажимает «Создать слой» (`Ctrl+L`) → в `Picture` добавляется новый `Layer`
   (id, name = "Layer N", visible = true). Слой становится **активным** (`EditorState::activeLayerId`).
2. Пользователь выделяет фигуру (клик по сцене) → `HitTestDeep` возвращает id (только для видимых слоёв).
3. Пользователь нажимает «Переместить в слой» (`Ctrl+Shift+L`), выбирает слой.
4. `InputRouter` создаёт `MoveToLayerCommand(picture, figureId, oldLayerId, newLayerId)`:
    - **Execute**: `Picture::ExtractFigureDeep(figureId)` (извлекает фигуру из старого слоя),
      `Picture::AddShape(figure, newLayerId)` (добавляет в новый слой, в конец).
    - **Unexecute**: `Picture::ExtractFigureDeep(figureId)` (из нового слоя),
      `Picture::AddShape(figure, oldLayerId, oldIndex)` (возвращает на старое место).
5. `Picture::NotifyPictureObservers()` → `ShapeListView` (дерево слоёв) и `TimelineView` обновляются.
6. **Undo** (`Ctrl+Z`): `CommandHistory::Undo()` → `MoveToLayerCommand::Unexecute` → фигура возвращается
   на прежний слой и индекс. `ApplySelection` восстанавливает выделение.
7. **Redo** (`Ctrl+Y`): `MoveToLayerCommand::Execute` → фигура снова перемещается.

## 5. Новые взаимодействия / изменения связей

| Связь                                  | Было                                          | Стало                                                  |
|----------------------------------------|-----------------------------------------------|--------------------------------------------------------|
| Input Routing → Document Model         | `HitTestDeep` (все фигуры)                    | `HitTestDeep` (только видимые слои) + `SetActiveLayer` |
| Input Routing → Command & Undo History | DeleteNested/Color/...                        | + `MoveToLayerCommand`                                 |
| Shortcuts → Command & Undo History     | DeleteNested/Group/Ungroup/Undo/Redo          | + `MoveToLayerCommand`                                 |
| Panel Views → Document Model           | `GetOrder`, `GetShapeDeep`                    | `GetLayers`, `GetShapesInLayer(layerId)`               |
| Scene View → Editor State              | `preview`, `selection`, `currentTime`         | + `activeLayerId`                                      |
| Document IO → Document Model           | `SaveService`/`LoadService` (фигуры и группы) | + сериализация `Layer` и `layerId`                     |
| Window / Composition Root → Scene View | `Render`, `setView(m_camera)`                 | + панель слоёв                                         |

## 6. Архитектурные риски и вопросы

1. **Слои vs группы: где хранить?**
   Слой — это **отдельная сущность** (не `GroupFigure`), потому что группа — это вложенная фигура,
   а слой — это «контейнер верхнего уровня» для Z-order. **Вопрос:** где хранить `layerId` —
   в `Figure` (поле) или в `Picture` (map `id → layerId`)? Если в `Figure`, то `GroupFigure`
   и `PrimitiveFigure` получат лишнее поле. Если в `Picture` — проще, но `Figure` не знает о слое.
   **Рекомендация:** хранить в `Picture` (map), чтобы не менять `Figure`.

2. **Скрытые слои и Hit-test.**
   Если слой скрыт, фигуры из него **не должны** попадать в `HitTestDeep`. Значит, `HitTestDeep`
   должен проверять `visible` слоя. **Вопрос:** как быть с выделением, если фигура была выделена,
   а слой скрыли? **Рекомендация:** при скрытии слоя — снимать выделение с его фигур.

3. **Сериализация и обратная совместимость.**
   Документы без слоёв (из ЛР4/ЛР5) должны загружаться. **Вопрос:** как отличить «старый» формат
   от «нового»? **Рекомендация:** если в файле нет `layer`, создавать «Default Layer» и класть
   все фигуры туда. Это сохраняет обратную совместимость.

4. **Z-order внутри слоя и между слоями.**
   Внутри слоя — порядок в `m_order` (как сейчас). Между слоями — порядок слоёв в `vector<Layer>`.
   **Вопрос:** как рисовать? **Рекомендация:** сначала рисуются фигуры нижнего слоя, потом верхнего.
   То есть `EditorView::Render` идёт по слоям (снизу вверх) и внутри каждого — по `m_order`.

5. **Слои и `EditorState::preview`.**
   `preview` привязан к `id` фигуры. При перемещении между слоями preview должен сбрасываться
   (иначе фигура «застрянет» в preview на старом слое). **Вопрос:** кто сбрасывает preview?
   **Рекомендация:** `InputRouter` / `ShapeTransformController` — при смене слоя.

6. **Дублирование точки создания команд.**
   Сейчас `DeleteCommand`/`DeleteNestedCommand` создаются в двух местах (`InputRouter` и `ShortcutController`).
   Для `MoveToLayerCommand` — та же проблема. **Вопрос:** вынести создание в общий хелпер?
   **Рекомендация:** да, иначе легко забыть одну из точек.

## 7. As-is / to-be (сводка)

| Аспект        | As-is                             | To-be                                                    |
|---------------|-----------------------------------|----------------------------------------------------------|
| `Picture`     | плоский `m_order` + `m_shapes`    | + `vector<Layer>`, `layerId` для каждой фигуры           |
| `EditorState` | выделение, preview, currentTime   | + `activeLayerId`, `IsShapeVisible` учитывает слой       |
| Команды       | DeleteNested, MoveShapes, ...     | + `MoveToLayerCommand`                                   |
| Input Routing | `HitTestDeep` (все)               | `HitTestDeep` (только видимые слои), + управление слоями |
| Shortcuts     | Delete, Ctrl+Z/Y, Group/Ungroup   | + Ctrl+L, Ctrl+H, Ctrl+Shift+L                           |
| Panel Views   | плоский список фигур              | дерево слоёв (сверху — слои, внутри — фигуры)            |
| Rendering     | DrawVisitor (рекурсивно)          | пропуск скрытых слоёв, порядок «снизу вверх»             |
| IO            | фигуры + группы                   | + Layer (id, name, visible), обратная совместимость      |
| Undo/Redo     | DeleteNested (Memento), Move, ... | + MoveToLayer (сохраняет oldLayerId, oldIndex)           |

## 8. Вывод

Добавление слоёв **не ломает** архитектуру, но вскрывает **отсутствие** понятия «уровень» в `Picture`.
Основные изменения локализованы в **Document Model** (новый `Layer` + `layerId` для фигур),
**Editor State** (активный слой, видимость), **Command & Undo** (`MoveToLayerCommand`),
**Input Routing** (hit-test только видимых слоёв) и **Panel Views** (дерево слоёв).
IO и Rendering — следствие. `DeleteNestedCommand` (Memento) **не конфликтует** со слоями:
он работает с `Picture` целиком, а слои — это надстройка над `m_order`.

Главные риски: где хранить `layerId` (в `Figure` или в `Picture`), обратная совместимость IO,
порядок отрисовки между слоями и сброс preview при смене слоя. Все они **проверяемы до реализации**
и не требуют переписывания архитектуры.