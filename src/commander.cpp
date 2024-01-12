#include "commander.h"

#include <cstdio>
#include <functional>
#include <iostream>
#include <sstream>

#include "def.h"
#include "dialog.h"
#include "error_dialog.h"
#include "file_info.h"
#include "fileutils.h"
#include "image_viewer.h"
#include "keyboard.h"
#include "resourceManager.h"
#include "screen.h"
#include "sdlutils.h"
#include "text_viewer.h"

#define SPLITTER_LINE_W static_cast<int>(1 * screen.ppu_x)
#define X_LEFT static_cast<int>(1 * screen.ppu_x)
#define X_RIGHT                                                                \
    (screen.actual_w / 2 + SPLITTER_LINE_W + static_cast<int>(1 * screen.ppu_x))

namespace {

SDL_Surface *DrawBackground() {
    SDL_Surface *bg = SDL_utils::createSurface(screen.actual_w, screen.actual_h);

    const int header_h = HEADER_H_PHYS;
    const int footer_h = FOOTER_H_PHYS;
    const int line_h = LINE_HEIGHT_PHYS;
    const int list_y = Y_LIST_PHYS;

    // Stripes
    const int stripes_h = screen.actual_h - header_h - footer_h;
    SDL_Rect rect = SDL_utils::makeRect(0, 0, screen.actual_w, screen.actual_h);
    const Uint32 bg_colors[2] = {SDL_MapRGB(bg->format, COLOR_BG_1), SDL_MapRGB(bg->format, COLOR_BG_2)};
    const std::size_t num_lines = (stripes_h - 1) / line_h + 1;
    for (std::size_t i = 0; i < num_lines; ++i) {
        rect.y = list_y + i * line_h;
        SDL_FillRect(bg, &rect, bg_colors[i % 2]);
    }

    // Top and bottom bars
    const auto bar_color = SDL_MapRGB(bg->format, COLOR_TITLE_BG);
    rect = SDL_utils::makeRect(0, 0, bg->w, list_y);
    SDL_FillRect(bg, &rect, bar_color);
    rect.y = bg->h - footer_h;
    SDL_FillRect(bg, &rect, bar_color);

    // Line in the middle
    rect = SDL_utils::Rect(screen.actual_w / 2, 0, SPLITTER_LINE_W, list_y);
    SDL_FillRect(bg, &rect, bg_colors[0]);
    rect.y = rect.h;
    rect.h = stripes_h;
    SDL_FillRect(bg, &rect, bar_color);
    rect.y += rect.h;
    rect.h = footer_h;
    SDL_FillRect(bg, &rect, bg_colors[0]);

    return bg;
}

} // namespace

CCommander::CCommander(const std::string &p_pathL, const std::string &p_pathR):
    CWindow::CWindow(),
    m_panelLeft(p_pathL, X_LEFT),
    m_panelRight(p_pathR, X_RIGHT),
    m_panelSource(NULL),
    m_panelTarget(NULL),
    m_background(DrawBackground())
{
    m_panelSource = &m_panelLeft;
    m_panelTarget = &m_panelRight;
}

CCommander::~CCommander(void)
{
    SDL_FreeSurface(m_background);
}

void CCommander::render(const bool p_focus) const
{
    INHIBIT(std::cout << "CCommander::render  fullscreen: " << isFullScreen() << "  focus: " << p_focus << std::endl;)
    // Draw background image
    SDL_utils::applySurface(0, 0, m_background, screen.surface);
    // Draw panels
    m_panelLeft.render(p_focus && (m_panelSource == &m_panelLeft));
    m_panelRight.render(p_focus && (m_panelSource == &m_panelRight));
}

void CCommander::onResize()
{
    SDL_FreeSurface(m_background);
    m_background = DrawBackground();
    m_panelRight.setX(X_RIGHT);
}

bool CCommander::keyPress(
    const SDL_Event &event, SDLC_Keycode key, ControllerButton button)
{
    CWindow::keyPress(event, key, button);
    const auto &c = config();
#if defined(KORIKI) && defined(USE_SDL2)
    if (key == c.key_system || button == c.gamepad_system) {	// added MENU for RG35xx
#elif defined(GARLIC)
    if (key == c.key_system || key == c.key_menu || button == c.gamepad_system) {	// added MENU for RG35xx Garlic
#elif defined(KORIKI) && defined(USE_SDL)
    if (key == c.key_system || button == c.gamepad_menu || button == c.gamepad_system) {	// added MENU for RG35xx Koriki
#endif
        if (openSystemMenu()) m_panelSource->refresh();
        return true;
    }
    if (key == c.key_up || button == c.gamepad_up) return actionUp();
    if (key == c.key_down || button == c.gamepad_down) return actionDown();
    if (key == c.key_pageup || button == c.gamepad_pageup)
        return actionPageUp();
    if (key == c.key_pagedown || button == c.gamepad_pagedown)
        return actionPageDown();
    if (key == c.key_home || button == c.gamepad_home) {
        return actionHome();
    }
    if (key == c.key_end || button == c.gamepad_end) {
        return actionEnd();
    }
    if (key == c.key_left || button == c.gamepad_left) {
        if (m_panelSource != &m_panelRight) return false;
        m_panelSource = &m_panelLeft;
        m_panelTarget = &m_panelRight;
        return true;
    }
    if (key == c.key_right || button == c.gamepad_right) {
        if (m_panelSource != &m_panelLeft) return false;
        m_panelSource = &m_panelRight;
        m_panelTarget = &m_panelLeft;
        return true;
    }
    if (key == c.key_open || button == c.gamepad_open) return itemMenu();
    if (key == c.key_parent || button == c.gamepad_parent)
        return m_panelSource->goToParentDir();
    if (key == c.key_operation || button == c.gamepad_operation) {
        // If there's no file in the select list, add current file
        if (m_panelSource->getSelectList().empty()
            && m_panelSource->getHighlightedItem() != "..")
            m_panelSource->addToSelectList(false);
        return operationMenu();
    }
    if (key == c.key_select || button == c.gamepad_select)
        return actionSelect();
    if (key == c.key_transfer || button == c.gamepad_transfer) {
        if (m_panelSource->isDirectoryHighlighted()
            && m_panelSource->getHighlightedItem() != "..") {
            return m_panelTarget->open(m_panelSource->getHighlightedItemFull());
        }
        return m_panelTarget->open(m_panelSource->getCurrentPath());
    }
    if (key == c.key_shot) {
        uint32_t timestamp = SDL_GetTicks();
        char filename[32];
        sprintf(filename, "screenshot-%d.bmp", timestamp);
        SDL_SaveBMP(SDL_GetVideoSurface(), filename);
        CDialog l_dialog("截图已保存", {}, []() { return Y_LIST_PHYS; });
        l_dialog.addOption("确定");
        l_dialog.init();
        l_dialog.execute();
        return true;
    }
    if (key == c.key_quit) {
        m_retVal = -1;
        return true;
    }
    return false;
}

bool CCommander::actionUp() { return m_panelSource->moveCursorUp(1); }
bool CCommander::actionDown() { return m_panelSource->moveCursorDown(1); }
bool CCommander::actionPageUp()
{
    return m_panelSource->moveCursorUp(NB_VISIBLE_LINES - 1);
}
bool CCommander::actionPageDown()
{
    return m_panelSource->moveCursorDown(NB_VISIBLE_LINES - 1);
}
bool CCommander::actionHome()
{
    return m_panelSource->moveCursorHome();
}
bool CCommander::actionEnd()
{
    return m_panelSource->moveCursorEnd();
}
bool CCommander::actionSelect() { return m_panelSource->addToSelectList(true); }

bool CCommander::keyHold()
{
    const auto &c = config();
    if (tick(c.key_up)) return actionUp();
    if (tick(c.key_down)) return actionDown();
    if (tick(c.key_pageup)) return actionPageUp();
    if (tick(c.key_pagedown)) return actionPageDown();
    if (tick(c.key_home)) {
        m_panelSource->moveCursorToVisibleLineIndex(0);
        return true;
    }
    if (tick(c.key_end)) {
        m_panelSource->moveCursorToVisibleLineIndex(NB_VISIBLE_LINES - 1);
        return true;
    }
    if (tick(c.key_select)) return actionSelect();
    return false;
}

#if SDL_VERSION_ATLEAST(2, 0, 0)
bool CCommander::gamepadHold(SDL_GameController *controller)
{
    const auto &c = config();
    if (tick(controller, c.gamepad_up)) return actionUp();
    if (tick(controller, c.gamepad_down)) return actionDown();
    if (tick(controller, c.gamepad_pageup)) return actionPageUp();
    if (tick(controller, c.gamepad_pagedown)) return actionPageDown();
    if (tick(controller, c.gamepad_select)) return actionSelect();
    return false;
}
#endif

CPanel *CCommander::focusPanelAt(int *x, int *y, bool *changed)
{
    if (*x < X_LEFT) return nullptr;
    CPanel *target;
    if (*x >= X_RIGHT) {
        target = &m_panelRight;
        *x -= X_RIGHT;
    } else {
        target = &m_panelLeft;
        *x -= X_LEFT;
    }
    if (m_panelSource != target) {
        m_panelTarget = m_panelSource;
        m_panelSource = target;
        *changed = true;
    }
    return target;
}

bool CCommander::mouseWheel(int dx, int dy)
{
    bool changed = false;
    int x, y;
    SDL_GetMouseState(&x, &y);
    CPanel *target = focusPanelAt(&x, &y, &changed);
    if (target == nullptr) return changed;
    if (dy > 0) return target->moveCursorUp(1) || changed;
    if (dy < 0) return target->moveCursorDown(1) || changed;
    return changed;
}

bool CCommander::mouseDown(int button, int x, int y)
{
    bool changed = false;
    CPanel *target = focusPanelAt(&x, &y, &changed);
    if (target == nullptr) return changed;
    const int line = target->getLineAt(x, y);
    switch (button)
    {
        case SDL_BUTTON_LEFT:
            if (line == -1) { openSystemMenu(); }
            else
            {
                target->moveCursorToVisibleLineIndex(line);
                itemMenu();
            }
            return true;
        case SDL_BUTTON_MIDDLE:
            if (line != -1)
            {
                target->moveCursorToVisibleLineIndex(line);
                target->addToSelectList(/*p_step=*/false);
                return true;
            }
            return changed;
        case SDL_BUTTON_RIGHT:
            if (m_panelSource->getSelectList().empty()) {
                if (line == -1) {
                    openSystemMenu();
                    return true;
                }
                target->moveCursorToVisibleLineIndex(line);
                target->addToSelectList(/*p_step=*/false);
                changed = true;
            }
            return operationMenu() || changed;
        case SDL_BUTTON_X1: return target->goToParentDir() || changed;
        case SDL_BUTTON_X2:
            if (target->isDirectoryHighlighted())
                return target->open() || changed;
            break;
    }
    return changed;
}

bool CCommander::itemMenu() const
{
    if (m_panelSource->isDirectoryHighlighted())
        return m_panelSource->open();
    // It's a file => open execute menu
    openExecuteMenu();
    return true;
}

bool CCommander::operationMenu() const
{
    if (m_panelSource->getSelectList().empty()) return false;
    if (openCopyMenu())
    {
        // Refresh file lists
        m_panelSource->refresh();
        m_panelTarget->refresh();
    }
    else
    {
        if (m_panelSource->getSelectList().size() == 1
            && (*m_panelSource->getSelectList().begin())
                == m_panelSource->getHighlightedIndex())
            m_panelSource->selectNone();
    }
    return true;
}

void CCommander::openHelpMenu(void) {
    CDialog *help_dialog = new CDialog("帮助", {}, []() { return Y_LIST_PHYS; });
    help_dialog->addLabel("Up     - 焦点移动到上一个选项");
    help_dialog->addLabel("Down   - 焦点移动到下一个选项");
    help_dialog->addLabel("Left   - 焦点移动到左侧窗口");
    help_dialog->addLabel("Right  - 焦点移动到右侧窗口");
    help_dialog->addLabel("A      - 选择/确认");
    help_dialog->addLabel("B      - 取消/返回");
    help_dialog->addLabel("X      - 选项菜单");
    help_dialog->addLabel("Y      - 主菜单");
    help_dialog->addLabel("L1     - 焦点移动到页顶");
    help_dialog->addLabel("R1     - 焦点移动到页尾");
    help_dialog->addLabel("L2     - 焦点移动到最上");
    help_dialog->addLabel("R2     - 焦点移动到最下");
    help_dialog->addLabel("Menu   - 主菜单");
    help_dialog->addLabel("Select - 勾选（用于多选）");
    help_dialog->addLabel("Start  - 在另一窗口打开");
    help_dialog->addOption("确定");
    help_dialog->init();
    help_dialog->execute();
    delete help_dialog;
}

const bool CCommander::openCopyMenu(void) const
{
    std::vector<std::function<bool()>> handlers;
    int l_dialogRetVal(0);
    // List of selected files
    std::vector<std::string> l_list;
    m_panelSource->getSelectList(l_list);
    {
        bool l_loop(false);
        std::ostringstream l_stream;
        l_stream << l_list.size() << " 已选择:";
        // File operation dialog
        CDialog l_dialog { l_stream.str(), {}, [this, &l_dialog]() {
                              return Y_LIST_PHYS
                                  + m_panelSource->getHighlightedIndexRelative()
                                  * l_dialog.line_height();
                          } };

        l_dialog.addOption(m_panelSource == &m_panelLeft ? "复制 >" : "< 复制");
        handlers.push_back([&]() {
            File_utils::copyFile(l_list, m_panelTarget->getCurrentPath());
            return true;
        });

        l_dialog.addOption(m_panelSource == &m_panelLeft ? "移动 >" : "< 移动");
        handlers.push_back([&]() {
            File_utils::moveFile(l_list, m_panelTarget->getCurrentPath());
            return true;
        });

        l_dialog.addOption(m_panelSource == &m_panelLeft ? "符号链接 >" : "< 符号链接");
        handlers.push_back([&]() {
            File_utils::symlinkFile(l_list, m_panelTarget->getCurrentPath());
            return true;
        });

        if (l_list.size() == 1) {
            // The rename option appears only if one item is selected
            l_dialog.addOption("重命名");
            handlers.push_back([&]() {
                CKeyboard l_keyboard(m_panelSource->getHighlightedItem());
                if (l_keyboard.execute() == 1 && !l_keyboard.getInputText().empty() && l_keyboard.getInputText() != m_panelSource->getHighlightedItem())
                {
                    File_utils::renameFile(m_panelSource->getHighlightedItemFull(), m_panelSource->getCurrentPath() + (m_panelSource->getCurrentPath() == "/" ? "" : "/") + l_keyboard.getInputText());
                    return true;
                }
                return false;
            });
        }

        l_dialog.addOption("删除");
        handlers.push_back([&]() {
            File_utils::removeFile(l_list);
            return true;
        });
        const int delete_option = handlers.size();

        l_dialog.addOption("磁盘占用");
        handlers.push_back([&]() {
            File_utils::diskUsed(l_list);
            return false;
        });

        l_dialog.init();
        do
        {
            l_loop = false;
            l_dialogRetVal = l_dialog.execute();
            if (l_dialogRetVal == delete_option) {
                CDialog l_dialog2 { "",
                    [&]() {
                        return l_dialog.getX() + l_dialog.width()
                            - l_dialog.border_x();
                    },
                    [&]() {
                        return l_dialog.getY() + l_dialog.border_y()
                            + (l_dialog.getHighlightedIndex() + 1)
                            * l_dialog.line_height();
                    } };
                l_dialog2.addOption("是");
                l_dialog2.addOption("否");
                l_dialog2.init();
                if (l_dialog2.execute() != 1)
                    l_loop = true;
            }
        }
        while (l_loop);
    }
    // Perform operation
    if (l_dialogRetVal > 0 && l_dialogRetVal <= handlers.size()) {
        return handlers[l_dialogRetVal - 1]();
    }
    return false;
}

const bool CCommander::openSystemMenu(void)
{
    bool l_ret(false);
    int l_dialogRetVal(0);
    // Selection dialog
    {
        CDialog l_dialog { "系统:", {}, [this, &l_dialog]() {
                              return Y_LIST_PHYS
                                  + m_panelSource->getHighlightedIndexRelative()
                                  * l_dialog.line_height();
                          } };
        l_dialog.addOption("选择全部");
        l_dialog.addOption("取消选择");
        l_dialog.addOption("新建目录");
        l_dialog.addOption("磁盘信息");
        l_dialog.addOption("帮助");
        l_dialog.addOption("退出");
#ifdef KORIKI
        l_dialog.addOption("Go /userdata/system/App");
        l_dialog.addOption("Go /userdata/Roms");
        l_dialog.addOption("Go /userdata/system/.simplemenu");
#endif
        l_dialog.init();
        l_dialogRetVal = l_dialog.execute();
    }
    switch (l_dialogRetVal)
    {
        case 1:
            // Select all
            m_panelSource->selectAll();
            break;
        case 2:
            // Select none
            m_panelSource->selectNone();
            break;
        case 3:
            // New dir
            {
                CKeyboard l_keyboard("");
                if (l_keyboard.execute() == 1 && !l_keyboard.getInputText().empty())
                {
                    File_utils::makeDirectory(m_panelSource->getCurrentPath() + (m_panelSource->getCurrentPath() == "/" ? "" : "/") + l_keyboard.getInputText());
                    m_panelSource->refresh();
                    l_ret = true;
                }
            }
            break;
        case 4:
            // Disk info
            File_utils::diskInfo();
            break;
        case 5: {
            // Help
            openHelpMenu();
            break;
        }
        case 6:
            // Quit
            m_retVal = -1;
            break;
#ifdef KORIKI
        case 7:
            m_panelSource->open("/userdata/system/App");
            break;
        case 8:
            m_panelSource->open("/userdata/Roms");
            break;
        case 9:
            m_panelSource->open("/userdata/system/.simplemenu");
            break;
#endif
        default:
            break;
    }
    return l_ret;
}

namespace {

enum class OpenFileResult
{
    CANCEL,
    VIEW,
    EXECUTE
};

OpenFileResult OpenFileDialog(const std::string &path,
    std::function<Sint16()> x_fn = {}, std::function<Sint16()> y_fn = {})
{
    const auto info = FileInfo::Get(path);

    if (File_utils::getLowercaseFileExtension(path) == "opk")
        return OpenFileResult::EXECUTE;

    if (!info.executable()) return OpenFileResult::VIEW;

    CDialog dlg { File_utils::getFileName(path) + ":" };
    std::vector<OpenFileResult> options { OpenFileResult::CANCEL };
    const auto add_option = [&](std::string text, OpenFileResult value) {
        dlg.addOption(text);
        options.push_back(value);
    };
    add_option("View", OpenFileResult::VIEW);
    add_option("Execute", OpenFileResult::EXECUTE);
    dlg.init();
    return options[dlg.execute()];
}

} // namespace

void CCommander::ViewFile(std::string &&path) const
{
    // Check size
    constexpr std::size_t kMaxFileSize = 16777216; // = 16 MB
    const auto file_size = File_utils::getFileSize(path);
    if (file_size > kMaxFileSize) {
        ErrorDialog(path, "Error:", "File too large (>16 MiB)");
        return;
    }
    ImageViewer image_viewer(m_panelSource);
    if (image_viewer.ok()) {
        image_viewer.execute();
        return;
    }

    TextViewer(path).execute();
}

void CCommander::openExecuteMenu(void) const
{
    switch (
        OpenFileDialog(m_panelSource->getHighlightedItemFull(), {}, [this]() {
            return Y_LIST
                + m_panelSource->getHighlightedIndexRelative() * LINE_HEIGHT;
        }))
    {
        case OpenFileResult::VIEW:
            ViewFile(m_panelSource->getHighlightedItemFull());
            break;
        case OpenFileResult::EXECUTE:
            File_utils::executeFile(m_panelSource->getHighlightedItemFull());
            break;
        case OpenFileResult::CANCEL:
            break;
    }
}

bool CCommander::isFullScreen(void) const
{
    return true;
}
