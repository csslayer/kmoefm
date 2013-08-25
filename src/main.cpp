#include "app.h"
#include <KAboutData>
#include <KCmdLineArgs>
#include <KDebug>

int main(int argc, char** argv)
{
    KAboutData aboutData("kmoefm", 0, ki18n("KMoeFM"),
                         "0.1", ki18n("Moe FM"),
                         KAboutData::License_GPL, ki18n("(c) 2013, Xuetian Weng"));
    aboutData.addAuthor(ki18n("Xuetian Weng"), ki18n("Author"), "wengxt@gmail.com");

    KCmdLineArgs::init(argc, argv, &aboutData);

    if (!App::start()) {
        kWarning() << "kmoefm is already running!";
        return 1;
    }
    App app;
    return app.exec();
}
