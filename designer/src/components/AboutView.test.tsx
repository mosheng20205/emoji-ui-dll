import { fireEvent, render, screen } from '@testing-library/react';
import AboutView from './AboutView';
import { openExternal } from '../utils/externalLink';

vi.mock('../utils/externalLink', () => ({
  openExternal: vi.fn().mockResolvedValue(undefined),
}));

describe('AboutView', () => {
  test('点击 QQ交流群 按钮时应打开外链', () => {
    render(<AboutView />);

    fireEvent.click(screen.getByRole('button', { name: 'QQ交流群' }));

    expect(openExternal).toHaveBeenCalledWith('https://qm.qq.com/q/NIZipXNsUS');
  });

  test('点击 GitHub Star 按钮时应打开仓库地址', () => {
    render(<AboutView />);

    fireEvent.click(screen.getByRole('button', { name: /GitHub Star/i }));

    expect(openExternal).toHaveBeenCalledWith('https://github.com/mosheng20205/emoji-ui-dll');
  });

  test('不应再显示广告位说明和展示数据区块', () => {
    render(<AboutView />);

    expect(screen.queryByRole('heading', { name: '广告位说明' })).not.toBeInTheDocument();
    expect(screen.queryByRole('heading', { name: '展示数据' })).not.toBeInTheDocument();
    expect(screen.queryByText('软件打开次数')).not.toBeInTheDocument();
  });

  test('应显示新版关于页核心信息卡', () => {
    render(<AboutView />);

    expect(screen.getByText('产品定位')).toBeInTheDocument();
    expect(screen.getByText('Emoji Window 可视化设计器')).toBeInTheDocument();
    expect(screen.getByText('GitHub 开源地址')).toBeInTheDocument();
    expect(screen.getByText('工具箱能力')).toBeInTheDocument();
    expect(screen.getByText('AI 配置与隐私')).toBeInTheDocument();
  });

  test('关于页的 QQ 交流群入口应显示企鹅图标而不是文字 QQ 徽标', () => {
    render(<AboutView />);

    expect(screen.getAllByTestId('qq-penguin-icon').length).toBeGreaterThan(0);
    expect(screen.queryByText('QQ', { selector: '.qq-icon-badge' })).not.toBeInTheDocument();
  });
});
