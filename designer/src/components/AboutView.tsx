import { openExternal } from '../utils/externalLink';
import QqPenguinIcon from './QqPenguinIcon';

export default function AboutView() {
  return (
    <section className="info-view" aria-label="关于页面">
      <div className="info-view-card about-redesign-shell">
        <section className="page-intro-card">
          <div className="page-intro-copy">
            <div className="page-intro-kicker">产品定位</div>
            <h1>关于</h1>
            <p>了解设计器定位、开源仓库、社群入口，以及 AI 本地配置相关说明。</p>
          </div>
          <div className="page-intro-badges">
            <span className="page-intro-badge">可视化搭建</span>
            <span className="page-intro-badge">多语言代码生成</span>
            <span className="page-intro-badge">本地 AI 配置</span>
          </div>
        </section>
        <section className="about-hero-card">
          <div className="about-hero-copy">
            <div className="about-kicker">Emoji Window Designer</div>
            <h2>Emoji Window 可视化设计器</h2>
            <p>面向 Windows 桌面场景的可视化界面设计工具，支持拖拽搭建窗口，并生成易语言、Python、C# 代码。</p>
          </div>
          <div className="about-hero-actions">
            <button
              className="header-btn settings-primary-btn"
              onClick={() => void openExternal('https://github.com/mosheng20205/emoji-ui-dll')}
            >
              GitHub Star
            </button>
            <button
              className="header-btn"
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
        </section>

        <div className="about-card-grid">
          <section className="about-feature-card">
            <h2>工具箱能力</h2>
            <p>左侧工具箱支持双击快速添加控件，也支持拖拽到画布中进行可视化布局，适合快速搭建桌面界面原型。</p>
          </section>

          <section className="about-feature-card">
            <h2>AI 配置与隐私</h2>
            <p>所有 AI 配置仅保存在本地，不同步云端。你可以直接查看开源代码，审查配置保存和网络请求逻辑。</p>
          </section>
        </div>

        <div className="about-section about-link-card">
          <h2>GitHub 开源地址</h2>
          <p className="about-link-url">https://github.com/mosheng20205/emoji-ui-dll</p>
          <p>如果这个项目对你有帮助，欢迎去 GitHub 点个 Star，这会帮助更多用户发现这个工具。</p>
        </div>

        <div className="about-card-grid">
          <section className="about-feature-card">
            <h2 className="qq-heading-inline">
              <span className="qq-icon-badge" aria-hidden="true">
                <QqPenguinIcon />
              </span>
              <span>QQ 交流群</span>
            </h2>
            <p>加入交流群可以获取更新通知、反馈使用问题，也方便共享界面设计和 AI 提示词经验。</p>
          </section>

          <section className="about-feature-card">
            <h2>开源仓库</h2>
            <p>项目已开源，欢迎查看实现细节、提出建议或参与后续能力扩展。</p>
          </section>
        </div>
      </div>
    </section>
  );
}
