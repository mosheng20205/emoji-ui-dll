import { useEffect, useMemo, useState } from 'react';
import { emojiCategories } from '../data/emojiCategories';
import { useDesignerStore } from '../store/designerStore';

interface Props {
  open: boolean;
  onClose: () => void;
}

const RECENT_STORAGE_KEY = 'designer_recent_emojis';
const RECENT_EMOJI_LIMIT = 20;

function loadRecentEmojis(): string[] {
  try {
    const saved = localStorage.getItem(RECENT_STORAGE_KEY);
    if (!saved) return [];
    const parsed = JSON.parse(saved);
    return Array.isArray(parsed) ? parsed.filter((item): item is string => typeof item === 'string') : [];
  } catch {
    return [];
  }
}

function saveRecentEmojis(emojis: string[]) {
  localStorage.setItem(RECENT_STORAGE_KEY, JSON.stringify(emojis));
}

export default function EmojiPicker({ open, onClose }: Props) {
  const { controls, selectedIds, updateControlProp } = useDesignerStore();
  const [activeCategory, setActiveCategory] = useState('smileys');
  const [selectedEmoji, setSelectedEmoji] = useState<string>('');
  const [copied, setCopied] = useState<string>('');
  const [search, setSearch] = useState('');
  const [recentEmojis, setRecentEmojis] = useState<string[]>([]);

  const selectedControl = useMemo(() => {
    if (selectedIds.length !== 1) return null;
    return controls.find((control) => control.id === selectedIds[0]) ?? null;
  }, [controls, selectedIds]);

  const canInsertToControl = !!selectedControl && Object.prototype.hasOwnProperty.call(selectedControl.props, 'emoji');
  const categories = useMemo(
    () =>
      emojiCategories.map((category) =>
        category.id === 'recent'
          ? { ...category, emojis: recentEmojis }
          : category
      ),
    [recentEmojis]
  );

  const currentCategory = categories.find((category) => category.id === activeCategory) ?? categories[0];
  const normalizedSearch = search.trim();
  const isSearching = normalizedSearch.length > 0;

  const visibleEmojis = useMemo(() => {
    if (!isSearching) {
      return currentCategory?.emojis ?? [];
    }

    const matched = new Set<string>();
    for (const category of categories) {
      const categoryMatches = category.label.includes(normalizedSearch);
      for (const emoji of category.emojis) {
        if (categoryMatches || emoji.includes(normalizedSearch)) {
          matched.add(emoji);
        }
      }
    }
    return Array.from(matched);
  }, [categories, currentCategory, isSearching, normalizedSearch]);

  useEffect(() => {
    if (!open) return;
    const loaded = loadRecentEmojis();
    setRecentEmojis(loaded);
    setActiveCategory(loaded.length > 0 ? 'recent' : 'smileys');
    setSearch('');
  }, [open]);

  if (!open) return null;

  const rememberEmoji = (emoji: string) => {
    setRecentEmojis((current) => {
      const next = [emoji, ...current.filter((item) => item !== emoji)].slice(0, RECENT_EMOJI_LIMIT);
      saveRecentEmojis(next);
      return next;
    });
  };

  const writeEmojiToClipboard = async (emoji: string) => {
    if (navigator.clipboard?.writeText) {
      await navigator.clipboard.writeText(emoji);
      return;
    }

    const input = document.createElement('textarea');
    input.value = emoji;
    document.body.appendChild(input);
    input.select();
    document.execCommand('copy');
    document.body.removeChild(input);
  };

  const copyEmoji = async (emoji: string) => {
    await writeEmojiToClipboard(emoji);
    rememberEmoji(emoji);
    setSelectedEmoji(emoji);
    setCopied(emoji);
    window.setTimeout(() => {
      setCopied('');
    }, 1200);
  };

  const insertEmoji = () => {
    if (!canInsertToControl || !selectedControl || !selectedEmoji) return;
    rememberEmoji(selectedEmoji);
    updateControlProp(selectedControl.id, 'emoji', selectedEmoji);
  };

  return (
    <div className="emoji-picker-popover" role="dialog" aria-label="Emoji 选择器">
      <div className="emoji-picker-header">
        <div className="emoji-picker-title">Emoji 选择器</div>
        <button className="emoji-picker-close" onClick={onClose} aria-label="关闭 Emoji 选择器">✕</button>
      </div>

      <div className="emoji-picker-search-wrap">
        <input
          className="emoji-picker-search"
          value={search}
          onChange={(event) => setSearch(event.target.value)}
          placeholder="搜索 Emoji 或分类"
          aria-label="搜索 Emoji 或分类"
        />
      </div>

      <div className="emoji-category-tabs">
        {categories.map((category) => (
          <button
            key={category.id}
            className={`emoji-category-tab ${category.id === activeCategory ? 'active' : ''}`}
            onClick={() => setActiveCategory(category.id)}
          >
            {category.label}
            {category.id === 'recent' && category.emojis.length > 0 ? ` (${category.emojis.length})` : ''}
          </button>
        ))}
      </div>

      <div className="emoji-picker-grid">
        {visibleEmojis.map((emoji) => (
          <button
            key={`${isSearching ? 'search' : currentCategory.id}-${emoji}`}
            className={`emoji-cell ${selectedEmoji === emoji ? 'selected' : ''}`}
            onClick={() => void copyEmoji(emoji)}
            aria-label={emoji}
            title={`点击复制 ${emoji}`}
          >
            {emoji}
          </button>
        ))}
        {visibleEmojis.length === 0 && (
          <div className="emoji-picker-empty">
            {isSearching ? '没有匹配的 Emoji' : '最近还没有使用记录'}
          </div>
        )}
      </div>

      <div className="emoji-picker-footer">
        <div className="emoji-picker-status">
          {copied ? `已复制 ${copied}` : isSearching ? `搜索到 ${visibleEmojis.length} 个结果` : '点击表情即可复制'}
        </div>
        {canInsertToControl && (
          <button className="emoji-insert-btn" onClick={insertEmoji} disabled={!selectedEmoji}>
            插入到当前控件
          </button>
        )}
      </div>
    </div>
  );
}
