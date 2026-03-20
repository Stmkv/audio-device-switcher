#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QIcon>
#include <QProcess>
#include <QString>

void run(const QString &program, const QStringList &args) {
    QProcess::execute(program, args);
}

void set_headset() {
    run("pactl", {"set-default-sink",
                  "alsa_output.usb-XiiSound_Technology_Corporation_Vault_White-00.iec958-stereo"});
    run("pactl", {"set-default-source",
                  "alsa_input.usb-XiiSound_Technology_Corporation_Vault_White-00.mono-fallback"});
}

void set_earbuds() {
    run("pactl", {"set-default-sink",
                  "alsa_output.usb-Generic_USB_Audio-00.iec958-stereo"});
    run("pactl", {"set-default-source",
                  "alsa_input.usb-MV-SILICON_fifine_Microphone_20190808-00.iec958-stereo"});
}

void set_speakers() {
    run("pactl", {"set-default-sink",
                  "alsa_output.pci-0000_01_00.1.hdmi-stereo"});
    run("pactl", {"set-default-source",
                  "alsa_input.usb-MV-SILICON_fifine_Microphone_20190808-00.iec958-stereo"});
}

QString get_current_sink() {
    QProcess process;
    process.start("pactl", {"get-default-sink"});
    process.waitForFinished();
    return process.readAllStandardOutput().trimmed();
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QSystemTrayIcon tray;
    tray.setIcon(QIcon::fromTheme("audio-headphones"));

    QMenu menu;

    QActionGroup *group = new QActionGroup(&menu);
    group->setExclusive(true);

    QAction *a1 = new QAction("🎧 Гарнитура");
    a1->setCheckable(true);
    QObject::connect(a1, &QAction::triggered, set_headset);

    QAction *a2 = new QAction("🎧 Затычки + микрофон");
    a2->setCheckable(true);
    QObject::connect(a2, &QAction::triggered, set_earbuds);

    QAction *a3 = new QAction("🔊 Монитор + микрофон");
    a3->setCheckable(true);
    QObject::connect(a3, &QAction::triggered, set_speakers);

    group->addAction(a1);
    group->addAction(a2);
    group->addAction(a3);

    menu.addAction(a1);
    menu.addAction(a2);
    menu.addAction(a3);

    menu.addSeparator();

    QAction *exit_action = new QAction("❌ Выход");
    QObject::connect(exit_action, &QAction::triggered, &app, &QApplication::quit);
    menu.addAction(exit_action);

    tray.setContextMenu(&menu);

    QString current = get_current_sink();

    if (current.contains("XiiSound")) {
        a1->setChecked(true);
    } else if (current.contains("Generic_USB_Audio")) {
        a2->setChecked(true);
    } else if (current.contains("pci-0000_01_00.1")) {
        a3->setChecked(true);
    }

    tray.show();

    return app.exec();
}