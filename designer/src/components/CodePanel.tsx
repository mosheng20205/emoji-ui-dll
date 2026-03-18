import { useEffect, useMemo, useRef, useState } from 'react';
import { Highlight, themes } from 'prism-react-renderer';
import { useDesignerStore } from '../store/designerStore';
import { generateCode, languageLabels, languageSyntax } from '../codegen';
import { CodeLanguage } from '../types/controls';
import { downloadTextFile } from '../utils/fileTransfer';

const langs: CodeLanguage[] = ['python', 'csharp', 'epl'];

export default function CodePanel() {
  const { window: win, controls, codeLanguage, setCodeLanguage, showCode, setShowCode, codePanelHeight, setCodePanelHeight } =
    useDesignerStore();
  const [copied, setCopied] = useState(false);
  const resizeStartRef = useRef<{ y: number; height: number } | null>(null);

  const code = useMemo(
    () => generateCode(codeLanguage, win, controls),
    [codeLanguage, win, controls]
  );

  const handleCopy = async () => {
    await navigator.clipboard.writeText(code);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  const handleDownload = async () => {
    const ext = codeLanguage === 'python' ? '.py' : codeLanguage === 'csharp' ? '.cs' : '.txt';
    await downloadTextFile(`emoji_window_app${ext}`, code, 'text/plain');
  };

  useEffect(() => {
    const onMouseMove = (event: MouseEvent) => {
      if (!resizeStartRef.current) return;
      const delta = resizeStartRef.current.y - event.clientY;
      setCodePanelHeight(resizeStartRef.current.height + delta);
    };

    const onMouseUp = () => {
      resizeStartRef.current = null;
    };

    window.addEventListener('mousemove', onMouseMove);
    window.addEventListener('mouseup', onMouseUp);
    return () => {
      window.removeEventListener('mousemove', onMouseMove);
      window.removeEventListener('mouseup', onMouseUp);
    };
  }, [setCodePanelHeight]);

  return (
    <div
      className={`code-panel ${showCode ? 'expanded' : 'collapsed'}`}
      style={showCode ? { height: `${codePanelHeight}px` } : undefined}
    >
      {showCode && (
        <div
          className="code-resizer"
          role="separator"
          aria-label="调整代码区高度"
          onMouseDown={(event) => {
            event.preventDefault();
            event.stopPropagation();
            resizeStartRef.current = { y: event.clientY, height: codePanelHeight };
          }}
        />
      )}
      <div className="code-header" onClick={() => setShowCode(!showCode)}>
        <div className="code-header-left">
          <span className="code-toggle">{showCode ? '▼' : '▶'}</span>
          <span className="code-header-title">生成代码</span>
          <span className="code-control-count">{controls.length} 个控件</span>
        </div>
        <div className="code-header-right" onClick={(e) => e.stopPropagation()}>
          <div className="lang-tabs">
            {langs.map((l) => (
              <button
                key={l}
                className={`lang-tab ${codeLanguage === l ? 'active' : ''}`}
                onClick={() => setCodeLanguage(l)}
              >
                {languageLabels[l]}
              </button>
            ))}
          </div>
          <button className="code-action-btn" onClick={handleCopy}>
            {copied ? '✓ 已复制' : '复制'}
          </button>
          <button className="code-action-btn" onClick={() => void handleDownload()}>
            下载
          </button>
        </div>
      </div>
      {showCode && (
        <div className="code-content">
          <Highlight theme={themes.vsDark} code={code} language={languageSyntax[codeLanguage]}>
            {({ style, tokens, getLineProps, getTokenProps }) => (
              <pre style={{ ...style, margin: 0, padding: '12px', overflow: 'auto', fontSize: 13, lineHeight: 1.5 }}>
                {tokens.map((line, i) => (
                  <div key={i} {...getLineProps({ line })}>
                    <span className="line-number">{i + 1}</span>
                    {line.map((token, key) => (
                      <span key={key} {...getTokenProps({ token })} />
                    ))}
                  </div>
                ))}
              </pre>
            )}
          </Highlight>
        </div>
      )}
    </div>
  );
}
