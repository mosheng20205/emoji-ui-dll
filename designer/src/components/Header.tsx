import { useEffect, useRef, useState } from 'react';
import { useDesignerStore } from '../store/designerStore';
import { downloadTextFile, promptJsonFile } from '../utils/fileTransfer';
import { openExternal } from '../utils/externalLink';
import { DEFAULT_ADS, type AdItem, fetchAds } from '../services/ads';
import { reportAdEvent } from '../services/statsReporter';
import { logAdsDebug } from '../utils/adsDebugLog';
import QqPenguinIcon from './QqPenguinIcon';

const ALIGN_ACTIONS = [
  { mode: 'left', icon: '⫷', label: '左对齐' },
  { mode: 'top', icon: '⫯', label: '上对齐' },
  { mode: 'hcenter', icon: '≡', label: '水平居中' },
  { mode: 'vcenter', icon: '⋮', label: '垂直居中' },
] as const;

export default function Header() {
  const { controls, selectedIds, clearAll, alignSelection, duplicateSelection, copySelection, pasteClipboard, runtimeConfig } =
    useDesignerStore();
  const [ads, setAds] = useState<AdItem[]>(DEFAULT_ADS);
  const [currentAdIndex, setCurrentAdIndex] = useState(0);
  const lastImpressionKeyRef = useRef<string | null>(null);

  const handleExport = async () => {
    const state = useDesignerStore.getState();
    const data = {
      window: state.window,
      controls: state.controls,
    };
    try {
      await downloadTextFile('design.json', JSON.stringify(data, null, 2));
    } catch {
      alert('导出失败：无法保存文件');
    }
  };

  const handleImport = async () => {
    await promptJsonFile((text) => {
      try {
        const data = JSON.parse(text);
        if (data.window && data.controls) {
          useDesignerStore.getState().loadDesign(data.window, data.controls);
        } else if (data.window) {
          useDesignerStore.getState().setWindow(data.window);
        }
      } catch {
        alert('导入失败：无效的JSON文件');
      }
    }, () => {
      alert('导入失败：文件读取失败');
    });
  };

  useEffect(() => {
    let mounted = true;
    void fetchAds(runtimeConfig).then((items) => {
      logAdsDebug(`Header:fetchAds resolved count=${items.length} ids=${items.map((item) => item.id).join(',') || '<none>'}`);
      if (mounted) {
        setAds(items);
        setCurrentAdIndex(0);
      }
    });
    return () => {
      mounted = false;
    };
  }, [runtimeConfig]);

  useEffect(() => {
    logAdsDebug(`Header:rotation effect adsLength=${ads.length}`);
    if (ads.length <= 1) {
      return;
    }
    const timer = window.setInterval(() => {
      setCurrentAdIndex((current) => {
        const next = (current + 1) % ads.length;
        logAdsDebug(`Header:rotation tick current=${current} next=${next} adsLength=${ads.length}`);
        return next;
      });
    }, 3000);
    return () => window.clearInterval(timer);
  }, [ads]);

  const currentAd = ads[currentAdIndex] ?? DEFAULT_ADS[0];

  useEffect(() => {
    const impressionKey = `${currentAd.id}:${currentAd.url}`;
    if (lastImpressionKeyRef.current === impressionKey) {
      return;
    }
    lastImpressionKeyRef.current = impressionKey;
    void reportAdEvent('impression', currentAd, runtimeConfig);
  }, [currentAd, runtimeConfig]);

  const handleAdClick = () => {
    void reportAdEvent('click', currentAd, runtimeConfig);
    void openExternal(currentAd.url);
  };

  return (
    <header className="app-header">
      <div className="header-brand">
        <span className="header-logo">🎨</span>
        <span className="header-title">Emoji Window 可视化设计器</span>
        <button
          className="header-btn header-link-btn"
          onClick={() => void openExternal('https://qm.qq.com/q/NIZipXNsUS')}
        >
          <span className="qq-link-content">
            <span className="qq-icon-badge" aria-hidden="true">
              <QqPenguinIcon />
            </span>
            <span>QQ交流群</span>
          </span>
        </button>
      </div>
      <div className="header-banner">
        <button
          className="header-banner-link"
          onClick={handleAdClick}
          title={currentAd.text}
        >
          {currentAd.text}
        </button>
      </div>
      <div className="header-actions">
        <span className="header-info">{controls.length} 个控件</span>
        {selectedIds.length > 0 && (
          <>
            <button className="header-btn" onClick={copySelection}>复制</button>
            <button className="header-btn" onClick={pasteClipboard}>粘贴</button>
            <button className="header-btn" onClick={duplicateSelection}>复制副本</button>
          </>
        )}
        {selectedIds.length > 1 && (
          <div className="align-toolbar" aria-label="对齐工具">
            {ALIGN_ACTIONS.map((action) => (
              <button
                key={action.mode}
                className="header-btn icon-btn"
                onClick={() => alignSelection(action.mode)}
                title={action.label}
                aria-label={action.label}
              >
                {action.icon}
              </button>
            ))}
          </div>
        )}
        <button className="header-btn" onClick={() => void handleImport()}>📂 导入</button>
        <button className="header-btn" onClick={() => void handleExport()}>💾 导出</button>
        <button className="header-btn danger" onClick={() => {
          if (confirm('确定清空所有控件？')) clearAll();
        }}>🗑️ 清空</button>
      </div>
    </header>
  );
}
