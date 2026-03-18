import { DesignControl, DesignWindow, CodeLanguage } from '../types/controls';
import { generatePython } from './python';
import { generateCSharp } from './csharp';
import { generateEpl } from './epl';

export function generateCode(
  lang: CodeLanguage,
  win: DesignWindow,
  controls: DesignControl[]
): string {
  switch (lang) {
    case 'python':
      return generatePython(win, controls);
    case 'csharp':
      return generateCSharp(win, controls);
    case 'epl':
      return generateEpl(win, controls);
  }
}

export const languageLabels: Record<CodeLanguage, string> = {
  python: 'Python',
  csharp: 'C#',
  epl: '易语言',
};

export const languageSyntax: Record<CodeLanguage, string> = {
  python: 'python',
  csharp: 'csharp',
  epl: 'plaintext',
};
