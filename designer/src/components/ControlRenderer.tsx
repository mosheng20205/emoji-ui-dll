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

    case 'tabcontrol': {
      const selBg = (p.selectedBgColor as string) || '#FFFFFF';
      const unselBg = (p.unselectedBgColor as string) || '#F5F7FA';
      const selText = (p.selectedTextColor as string) || '#409EFF';
      const unselText = (p.unselectedTextColor as string) || '#606266';
      const indColor = (p.indicatorColor as string) || '#409EFF';
      const tabW = (p.tabItemWidth as number) || 120;
      const tabH = (p.tabItemHeight as number) || 34;
      const padH = (p.paddingH as number) ?? 2;
      const padV = (p.paddingV as number) ?? 0;
      const closable = !!p.closable;
      const tabPos = (p.tabPosition as number) ?? 0;
      const tabAlign = (p.tabAlignment as number) ?? 0;
      const activeIdx = Number(p.activeTab) || 0;
      const tabItems = ((p.tabs as string) || '').split('\n').filter(Boolean);
      const isVertical = tabPos === 2 || tabPos === 3;
      const isBottom = tabPos === 1;
      const isRight = tabPos === 3;

      const tabBar = (
        <div style={{
          display: 'flex',
          flexDirection: isVertical ? 'column' : 'row',
          justifyContent: tabAlign === 1 ? 'center' : tabAlign === 2 ? 'flex-end' : 'flex-start',
          background: unselBg,
          ...(isVertical
            ? { borderRight: isRight ? 'none' : `2px solid ${indColor}`, borderLeft: isRight ? `2px solid ${indColor}` : 'none' }
            : { borderBottom: isBottom ? 'none' : `2px solid ${indColor}`, borderTop: isBottom ? `2px solid ${indColor}` : 'none' }),
          flexShrink: 0,
        }}>
          {tabItems.map((tab, i) => {
            const isActive = i === activeIdx;
            return (
              <div key={i} style={{
                width: isVertical ? undefined : tabW,
                height: isVertical ? tabH : undefined,
                minHeight: isVertical ? undefined : tabH,
                padding: `${padV}px ${padH + (closable ? 18 : 0)}px ${padV}px ${padH}px`,
                fontSize: (p.fontSize as number) || 13,
                fontFamily: (p.fontName as string) || 'Microsoft YaHei UI',
                fontWeight: p.bold ? 'bold' : 'normal',
                background: isActive ? selBg : 'transparent',
                color: isActive ? selText : unselText,
                cursor: 'default',
                display: 'flex', alignItems: 'center', justifyContent: 'center',
                position: 'relative',
                boxSizing: 'border-box',
                whiteSpace: 'nowrap', overflow: 'hidden', textOverflow: 'ellipsis',
                ...(isVertical
                  ? {
                      borderLeft: !isRight && isActive ? `2px solid ${indColor}` : '2px solid transparent',
                      borderRight: isRight && isActive ? `2px solid ${indColor}` : '2px solid transparent',
                    }
                  : {
                      borderTop: !isBottom && isActive ? `2px solid ${indColor}` : '2px solid transparent',
                      borderBottom: isBottom && isActive ? `2px solid ${indColor}` : '2px solid transparent',
                    }),
              }}
              onMouseDown={(event) => {
                event.stopPropagation();
                onTabSelect?.(i);
              }}>
                <span style={{ overflow: 'hidden', textOverflow: 'ellipsis' }}>{tab}</span>
                {closable && (
                  <span style={{
                    position: 'absolute',
                    right: 4, top: '50%', transform: 'translateY(-50%)',
                    fontSize: 10, color: '#999', lineHeight: 1,
                    width: 14, height: 14, display: 'flex', alignItems: 'center', justifyContent: 'center',
                    borderRadius: '50%',
                  }}>×</span>
                )}
              </div>
            );
          })}
        </div>
      );

      const contentArea = (
        <div style={{ flex: 1, background: selBg, border: '1px solid #e4e7ed', borderTop: isBottom || isVertical ? undefined : 'none', borderBottom: isBottom ? 'none' : undefined, borderLeft: isRight ? 'none' : undefined, borderRight: !isRight && isVertical ? 'none' : undefined }} />
      );

      return (
        <div style={{ width: '100%', height: '100%', display: 'flex', flexDirection: isVertical ? (isRight ? 'row' : 'row') : 'column' }}>
          {isBottom || isRight ? (
            <>{contentArea}{tabBar}</>
          ) : (
            <>{tabBar}{contentArea}</>
          )}
        </div>
      );
    }

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

    case 'datetimepicker': {
      const prec = (p.precision as number) ?? 4;
      let sample = '2024-06-15';
      if (prec >= 2) sample += ' 14';
      if (prec >= 3) sample += ':30';
      if (prec >= 4) sample += ':45';
      return (
        <div style={{
          width: '100%', height: '100%',
          border: `1px solid ${(p.borderColor as string) || '#DCDFE6'}`,
          borderRadius: 4,
          background: (p.bgColor as string) || '#fff',
          color: (p.fgColor as string) || '#606266',
          padding: '4px 32px 4px 8px',
          fontSize: (p.fontSize as number) || 14,
          fontFamily: (p.fontName as string) || 'Microsoft YaHei UI',
          fontWeight: p.bold ? 'bold' : 'normal',
          fontStyle: p.italic ? 'italic' : 'normal',
          textDecoration: p.underline ? 'underline' : 'none',
          display: 'flex', alignItems: 'center',
          position: 'relative', userSelect: 'none',
          opacity: p.enabled === false ? 0.5 : 1,
          visibility: p.visible === false ? 'hidden' as const : 'visible',
        }}>
          <span style={{ overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' }}>{sample}</span>
          <span style={{
            position: 'absolute', right: 6, top: '50%', transform: 'translateY(-50%)',
            width: 22, height: 22, display: 'flex', alignItems: 'center', justifyContent: 'center',
            fontSize: 10, color: '#909399', borderLeft: '1px solid #ebeef5',
          }}>▼</span>
        </div>
      );
    }

    case 'treeview':
    case 'treeview_sidebar': {
      const isSidebar = type === 'treeview_sidebar';
      const rowH = isSidebar ? Math.max(20, (p.rowHeight as number) || 38) : 28;
      const gap = isSidebar ? Math.max(0, (p.itemSpacing as number) ?? 2) : 0;
      const selBg = (p.selectedBgColor as string) || '#335EEA';
      const selFg = (p.selectedForeColor as string) || '#fff';
      const Row = ({
        indent,
        label,
        chevron,
        selected,
      }: { indent: number; label: string; chevron: string; selected?: boolean }) => (
        <div style={{
          display: 'flex', alignItems: 'center', justifyContent: 'space-between',
          minHeight: rowH,
          marginBottom: gap,
          paddingLeft: 8 + indent * 14,
          paddingRight: 8,
          borderRadius: 4,
          background: selected ? selBg : 'transparent',
          color: selected ? selFg : ((p.fgColor as string) || '#303133'),
          fontSize: (p.fontSize as number) || 13,
        }}>
          <span style={{ overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' }}>{label}</span>
          {isSidebar ? <span style={{ opacity: 0.55, fontSize: 11 }}>{chevron}</span> : null}
        </div>
      );
      return (
        <div style={{
          width: '100%', height: '100%',
          border: '1px solid #DCDFE6', borderRadius: 4,
          background: (p.bgColor as string) || '#fff',
          padding: isSidebar ? '6px 4px' : 4, overflow: 'hidden',
          display: 'flex', flexDirection: 'column',
        }}>
          {isSidebar ? (
            <>
              <Row indent={0} label="商户中心" chevron="" />
              <Row indent={0} label="订单管理" chevron="∨" />
              <Row indent={1} label="全部订单" chevron="" selected />
              <Row indent={1} label="售后单" chevron="" />
              <Row indent={0} label="通道管理" chevron="›" />
            </>
          ) : (
            <>
              <div style={{ padding: '2px 4px', color: (p.fgColor as string) || '#303133', fontSize: (p.fontSize as number) || 13 }}>📁 根节点</div>
              <div style={{ padding: '2px 4px 2px 20px', color: (p.fgColor as string) || '#303133', fontSize: (p.fontSize as number) || 13 }}>📄 子节点 1</div>
              <div style={{ padding: '2px 4px 2px 20px', color: (p.fgColor as string) || '#303133', fontSize: (p.fontSize as number) || 13 }}>📁 子节点 2</div>
              <div style={{ padding: '2px 4px 2px 36px', color: (p.fgColor as string) || '#303133', fontSize: (p.fontSize as number) || 13 }}>📄 孙节点</div>
            </>
          )}
        </div>
      );
    }

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
