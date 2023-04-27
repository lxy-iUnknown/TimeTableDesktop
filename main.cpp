#include "pch.h"
#include "time_table_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(STRING_LITERAL(":/icon.png")));
    TimeTableWindow w;
    if (argc > 1)
    {
        w.ParseArguments(app);
    }
    w.show();
    return app.exec();
}
