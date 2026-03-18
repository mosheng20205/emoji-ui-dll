import { DesignControl } from '../types/controls';

interface Props {
  control: DesignControl;
  onTabSelect?: (index: number) => void;
}

export default function ControlRenderer({ control, onTabSelect }: Props) {
  const { type, props, width, height } = control;
  const p = props as Record<string, unknown>;

  switch (type) {
    case 'button':
      return (
        <div style={{
          width: '100%', height: '100%',
          background: (p.bgColor as string) || '#409EFF',
          color: (p.fgColor as string) || '#fff',
          display: 'flex', alignItems: 'center', justifyContent: 'center',
          borderRadius: 4, fontSize: (p.fontSize as number) || 13,
          fontWeight: p.bold ? 'bold' : 'normal',
          fontFamily: (p.fontName as string) || 'Microsoft YaHei UI',
          cursor: 'default', userSelect: 'none', gap: 4,
          boxShadow: '0 1px 3px rgba(0,0,0,0.15)',
        }}>
          {p.emoji ? <span>{String(p.emoji)}</span> : null}
          <span>{(p.text as string) || '按钮'}</span>
        </div>
      );

    case 'label':
      return (
        <div style={{
          width: '100%', height: '100%',
          color: (p.fgColor as string) || '#303133',
          backgroundColor: (p.bgColor as string) || '#FFFFFF',
          display: 'flex', alignItems: 'center',
          justifyContent: p.alignment === 1 ? 'center' : p.alignment === 2 ? 'flex-end' : 'flex-start',
          fontSize: (p.fontSize as number) || 13,
          fontWeight: p.bold ? 'bold' : 'normal',
          fontFamily: (p.fontName as string) || 'Microsoft YaHei UI',
          overflow: 'hidden', whiteSpace: p.wordWrap ? 'normal' : 'nowrap',
          padding: '0 6px',
        }}>
          {(p.text as string) || '标签'}
        </div>
      );

    case 'editbox':
      return (
        <div style={{
          width: '100%', height: '100%',
          border: p.showBorder !== false ? '1px solid #DCDFE6' : 'none',
          borderRadius: 4,
          background: (p.bgColor as string) || '#fff',
          color: (p.fgColor as string) || '#303133',
          padding: '4px 8px',
          fontSize: (p.fontSize as number) || 13,
          fontFamily: (p.fontName as string) || 'Microsoft YaHei UI',
          display: 'flex', alignItems: p.vertCenter !== false ? 'center' : 'flex-start',
          overflow: 'hidden',
        }}>
          <span style={{ opacity: 0.5 }}>
            {p.password ? '••••••' : ((p.text as string) || (p.multiline ? '多行编辑框' : '编辑框'))}
          </span>
        </div>
      );

    case 'checkbox':
      return (
        <div style={{
          width: '100%', height: '100%',
          display: 'flex', alignItems: 'center', gap: 6,
          color: (p.fgColor as string) || '#303133',
          fontSize: (p.fontSize as number) || 13,
          fontFamily: (p.fontName as string) || 'Microsoft YaHei UI',
        }}>
          <div style={{
            width: 16, height: 16, borderRadius: 3,
            border: `2px solid ${p.checked ? (p.checkColor as string || '#409EFF') : '#DCDFE6'}`,
            background: p.checked ? (p.checkColor as string || '#409EFF') : '#fff',
            display: 'flex', alignItems: 'center', justifyContent: 'center',
            color: '#fff', fontSize: 11, flexShrink: 0,
          }}>
            {p.checked ? '✓' : null}
          </div>
          <span>{String(p.text || '复选框')}</span>
        </div>
      );

    case 'radiobutton':
      return (
        <div style={{
          width: '100%', height: '100%',
          display: 'flex', alignItems: 'center', gap: 6,
          color: (p.fgColor as string) || '#303133',
          fontSize: (p.fontSize as number) || 13,
          fontFamily: (p.fontName as string) || 'Microsoft YaHei UI',
        }}>
          <div style={{
            width: 16, height: 16, borderRadius: '50%',
            border: `2px solid ${p.checked ? (p.radioColor as string || '#409EFF') : '#DCDFE6'}`,
            display: 'flex', alignItems: 'center', justifyContent: 'center',
            flexShrink: 0,
          }}>
            {p.checked ? <div style={{
              width: 8, height: 8, borderRadius: '50%',
              background: (p.radioColor as string) || '#409EFF',
            }} /> : null}
          </div>
          <span>{(p.text as string) || '单选按钮'}</span>
        </div>
      );

    case 'progressbar':
      return (
        <div style={{
          width: '100%', height: '100%',
          background: (p.bgColor as string) || '#EBEEF5',
          borderRadius: 4, overflow: 'hidden', position: 'relative',
        }}>
          <div style={{
            width: `${(p.value as number) || 50}%`, height: '100%',
            background: (p.fgColor as string) || '#409EFF',
            borderRadius: 4, transition: 'width 0.3s',
          }} />
          {p.showText !== false && (
            <span style={{
              position: 'absolute', top: 0, left: 0, right: 0, bottom: 0,
              display: 'flex', alignItems: 'center', justifyContent: 'center',
              fontSize: 11, color: '#fff', fontWeight: 'bold',
              textShadow: '0 1px 2px rgba(0,0,0,0.3)',
            }}>{(p.value as number) || 50}%</span>
          )}
        </div>
      );

    case 'combobox':
      return (
        <div style={{
          width: '100%', height: '100%',
          border: '1px solid #DCDFE6', borderRadius: 4,
          background: (p.bgColor as string) || '#fff',
          color: (p.fgColor as string) || '#303133',
          padding: '4px 28px 4px 8px',
          fontSize: (p.fontSize as number) || 13,
          display: 'flex', alignItems: 'center',
          position: 'relative',
        }}>
          <span style={{ opacity: 0.6 }}>
            {((p.items as string) || '').split('\n')[0] || '请选择'}
          </span>
          <span style={{
            position: 'absolute', right: 8, top: '50%', transform: 'translateY(-50%)',
            fontSize: 10, color: '#999',
          }}>▼</span>
        </div>
      );

    case 'listbox':
      return (
        <div style={{
          width: '100%', height: '100%',
          border: '1px solid #DCDFE6', borderRadius: 4,
          background: (p.bgColor as string) || '#fff',
          color: (p.fgColor as string) || '#303133',
          fontSize: (p.fontSize as number) || 13,
          overflow: 'hidden',
        }}>
          {((p.items as string) || '').split('\n').filter(Boolean).slice(0, Math.floor(height / 26)).map((item, i) => (
            <div key={i} style={{
              padding: '3px 8px',
              background: i === 0 ? '#ecf5ff' : 'transparent',
              borderBottom: '1px solid #f0f0f0',
              whiteSpace: 'nowrap', overflow: 'hidden', textOverflow: 'ellipsis',
            }}>{item}</div>
          ))}
        </div>
      );

    case 'groupbox':
      return (
        <div style={{
          width: '100%', height: '100%',
          border: `1px solid ${(p.borderColor as string) || '#DCDFE6'}`,
          borderRadius: 4, position: 'relative',
          background: 'transparent',
        }}>
          <span style={{
            position: 'absolute', top: -10, left: 12,
            background: (p.bgColor as string) || '#fff',
            padding: '0 6px',
            color: (p.fgColor as string) || '#303133',
            fontSize: (p.fontSize as number) || 13,
            fontWeight: p.bold ? 'bold' : 'normal',
          }}>{(p.text as string) || '分组框'}</span>
        </div>
      );

    case 'picturebox':
      return (
        <div style={{
          width: '100%', height: '100%',
          border: `1px solid ${(p.borderColor as string) || '#DCDFE6'}`,
          background: (p.bgColor as string) || '#F5F7FA',
          display: 'flex', alignItems: 'center', justifyContent: 'center',
          borderRadius: 4, color: '#909399', fontSize: 28,
        }}>🖼️</div>
      );

    case 'tabcontrol':
      return (
        <div style={{ width: '100%', height: '100%', display: 'flex', flexDirection: 'column' }}>
          <div style={{
            display: 'flex', borderBottom: '2px solid #409EFF',
            background: '#f5f7fa',
          }}>
            {((p.tabs as string) || '').split('\n').filter(Boolean).map((tab, i) => (
              <div key={i} style={{
                padding: '4px 14px', fontSize: 12,
                background: i === (Number(p.activeTab) || 0) ? '#fff' : 'transparent',
                borderTop: i === (Number(p.activeTab) || 0) ? '2px solid #409EFF' : '2px solid transparent',
                color: i === (Number(p.activeTab) || 0) ? '#409EFF' : '#606266',
                cursor: 'default',
              }}
              onMouseDown={(event) => {
                event.stopPropagation();
                onTabSelect?.(i);
              }}>{tab}</div>
            ))}
          </div>
          <div style={{ flex: 1, background: '#fff', border: '1px solid #e4e7ed', borderTop: 'none' }} />
        </div>
      );

    case 'datagridview':
      return (
        <div style={{ width: '100%', height: '100%', display: 'flex', flexDirection: 'column', overflow: 'hidden' }}>
          <div style={{
            display: 'flex',
            background: (p.headerColor as string) || '#409EFF',
            color: '#fff', fontSize: 12, flexShrink: 0,
          }}>
            {((p.columns as string) || '').split('\n').filter(Boolean).map((col, i) => (
              <div key={i} style={{
                flex: 1, padding: '5px 8px',
                borderRight: '1px solid rgba(255,255,255,0.2)',
              }}>{col}</div>
            ))}
          </div>
          {[0, 1, 2].map((row) => (
            <div key={row} style={{
              display: 'flex', fontSize: 12,
              background: row % 2 ? '#fafafa' : '#fff',
              borderBottom: '1px solid #ebeef5',
            }}>
              {((p.columns as string) || '').split('\n').filter(Boolean).map((_, i) => (
                <div key={i} style={{
                  flex: 1, padding: '4px 8px', color: '#606266',
                  borderRight: '1px solid #ebeef5',
                }}>—</div>
              ))}
            </div>
          ))}
        </div>
      );

    case 'treeview':
      return (
        <div style={{
          width: '100%', height: '100%',
          border: '1px solid #DCDFE6', borderRadius: 4,
          background: (p.bgColor as string) || '#fff',
          color: (p.fgColor as string) || '#303133',
          fontSize: (p.fontSize as number) || 13,
          padding: 4, overflow: 'hidden',
        }}>
          <div style={{ padding: '2px 4px' }}>📁 根节点</div>
          <div style={{ padding: '2px 4px 2px 20px' }}>📄 子节点 1</div>
          <div style={{ padding: '2px 4px 2px 20px' }}>📁 子节点 2</div>
          <div style={{ padding: '2px 4px 2px 36px' }}>📄 孙节点</div>
        </div>
      );

    default:
      return (
        <div style={{
          width, height, background: '#eee',
          display: 'flex', alignItems: 'center', justifyContent: 'center',
          border: '1px dashed #ccc', fontSize: 12, color: '#999',
        }}>{type}</div>
      );
  }
}
