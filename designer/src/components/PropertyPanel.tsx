import { useDesignerStore } from '../store/designerStore';
import { getControlDef } from '../data/controlDefs';

export default function PropertyPanel() {
  const { controls, selectedIds, window: win, setWindow, updateControl, updateControlProp, removeControl } =
    useDesignerStore();
  const selected = selectedIds.length === 1
    ? controls.find((c) => c.id === selectedIds[0])
    : null;

  if (selectedIds.length > 1) {
    return (
      <div className="property-panel">
        <div className="prop-title">多选属性</div>
        <div className="prop-section">
          <div className="multi-select-panel">
            已选中 {selectedIds.length} 个控件
          </div>
        </div>
      </div>
    );
  }

  if (!selected) {
    return (
      <div className="property-panel">
        <div className="prop-title">窗口属性</div>
        <div className="prop-section">
          <div className="prop-row">
            <label>标题</label>
            <input value={win.title} onChange={(e) => setWindow({ title: e.target.value })} />
          </div>
          <div className="prop-row">
            <label>Emoji</label>
            <input value={win.emoji} onChange={(e) => setWindow({ emoji: e.target.value })} style={{ width: 50 }} />
          </div>
          <div className="prop-row">
            <label>宽度</label>
            <input type="number" value={win.width} min={200}
              onChange={(e) => setWindow({ width: +e.target.value })} />
          </div>
          <div className="prop-row">
            <label>高度</label>
            <input type="number" value={win.height} min={150}
              onChange={(e) => setWindow({ height: +e.target.value })} />
          </div>
          <div className="prop-row">
            <label>标题栏色</label>
            <input type="color" value={win.titlebarColor}
              onChange={(e) => setWindow({ titlebarColor: e.target.value })} />
          </div>
          <div className="prop-row">
            <label>背景色</label>
            <input type="color" value={win.bgColor}
              onChange={(e) => setWindow({ bgColor: e.target.value })} />
          </div>
        </div>
      </div>
    );
  }

  const def = getControlDef(selected.type);
  if (!def) return null;

  return (
    <div className="property-panel">
      <div className="prop-title">
        <span>{def.icon} {def.label}</span>
        <button className="prop-delete-btn" onClick={() => removeControl(selected.id)} title="删除控件">🗑️</button>
      </div>
      <div className="prop-section">
        <div className="prop-row">
          <label>名称</label>
          <input value={selected.name}
            onChange={(e) => updateControl(selected.id, { name: e.target.value })} />
        </div>
      </div>
      <div className="prop-section">
        {def.properties.map((prop) => {
          const val = ['x', 'y', 'width', 'height'].includes(prop.key)
            ? selected[prop.key as keyof typeof selected]
            : selected.props[prop.key];

          return (
            <div className="prop-row" key={prop.key}>
              <label>{prop.label}</label>
              {prop.type === 'string' && (
                prop.key === 'items' || prop.key === 'tabs' || prop.key === 'columns' ? (
                  <textarea
                    value={(val as string) || ''}
                    rows={3}
                    onChange={(e) => {
                      updateControlProp(selected.id, prop.key, e.target.value);
                    }}
                  />
                ) : (
                  <input
                    value={(val as string) || ''}
                    onChange={(e) => {
                      if (['x', 'y', 'width', 'height'].includes(prop.key)) {
                        updateControl(selected.id, { [prop.key]: e.target.value } as Record<string, unknown>);
                      } else {
                        updateControlProp(selected.id, prop.key, e.target.value);
                      }
                    }}
                  />
                )
              )}
              {prop.type === 'emoji' && (
                <input
                  value={(val as string) || ''}
                  placeholder="输入Emoji"
                  onChange={(e) => updateControlProp(selected.id, prop.key, e.target.value)}
                  style={{ width: 80 }}
                />
              )}
              {prop.type === 'number' && (
                <input
                  type="number"
                  value={(val as number) ?? 0}
                  min={prop.min}
                  max={prop.max}
                  onChange={(e) => {
                    const v = +e.target.value;
                    if (['x', 'y', 'width', 'height'].includes(prop.key)) {
                      updateControl(selected.id, { [prop.key]: v } as Record<string, unknown>);
                    } else {
                      updateControlProp(selected.id, prop.key, v);
                    }
                  }}
                />
              )}
              {prop.type === 'boolean' && (
                <input
                  type="checkbox"
                  checked={!!val}
                  onChange={(e) => updateControlProp(selected.id, prop.key, e.target.checked)}
                />
              )}
              {prop.type === 'color' && (
                <div className="color-input">
                  <input
                    type="color"
                    value={(val as string) || '#000000'}
                    onChange={(e) => updateControlProp(selected.id, prop.key, e.target.value)}
                  />
                  <span className="color-hex">{(val as string) || '#000000'}</span>
                </div>
              )}
              {prop.type === 'select' && (
                <select
                  value={val as string | number}
                  onChange={(e) => {
                    const v = isNaN(+e.target.value) ? e.target.value : +e.target.value;
                    updateControlProp(selected.id, prop.key, v);
                  }}
                >
                  {prop.options?.map((opt) => (
                    <option key={opt.value} value={opt.value}>{opt.label}</option>
                  ))}
                </select>
              )}
            </div>
          );
        })}
      </div>
    </div>
  );
}
