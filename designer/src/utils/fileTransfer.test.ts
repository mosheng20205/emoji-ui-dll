import { downloadTextFileWithAdapter } from './fileTransfer';

describe('fileTransfer', () => {
  test('Tauri 环境下导出应走原生保存与写文件', async () => {
    const save = vi.fn().mockResolvedValue('C:/temp/design.json');
    const writeTextFile = vi.fn().mockResolvedValue(undefined);

    await downloadTextFileWithAdapter(
      'design.json',
      '{"ok":true}',
      'application/json',
      { save, writeTextFile }
    );

    expect(save).toHaveBeenCalled();
    expect(writeTextFile).toHaveBeenCalledWith('C:/temp/design.json', '{"ok":true}');
  });

  test('用户取消保存时不应写文件', async () => {
    const save = vi.fn().mockResolvedValue(null);
    const writeTextFile = vi.fn();

    await downloadTextFileWithAdapter(
      'design.json',
      '{"ok":true}',
      'application/json',
      { save, writeTextFile }
    );

    expect(writeTextFile).not.toHaveBeenCalled();
  });
});
