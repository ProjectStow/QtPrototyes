#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>
#include <QTextStream>
#include <QFileInfo>

void setupParser(QCommandLineParser& parser);
bool checkArguments(QCommandLineParser& parser);
void outputArgumentErrorOrHelp(QString msg);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("CommandLine");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    setupParser(parser);
    if (!checkArguments(parser))
    {
        return -1;
    }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "CommandLine_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    return 1;

    // Right noew we do not need an Event Loop.
    //return a.exec();
}

void outputArgumentErrorOrHelp(QString msg)
{
    QTextStream output(stdout);
    output << msg << Qt::endl;
    return;
}

void setupParser(QCommandLineParser& parser)
{
    //TODO: Add Translation

    parser.addOption({{"i","input"},"Input resource file name <filename>.","resources.rc"});
    //Note: We removed the "-h" option since help uses it
    parser.addOption({"header","Output header file name <filename>.","strings.h"});
    parser.addOption({{"t","translation"},"Output translation file <filename>.","translation.ts"});
    parser.addOption({{"a","append"},"Append/Insert into target files."});
    //Note: We removed the "-v" option since version uses it
    parser.addOption({"verbose","Verbose mode  (Exlusive with Silent mode)."});
    parser.addOption({{"s","silent"},"Silent mode (Exclusive with Verbose mode)."});
    parser.addHelpOption();
    parser.addVersionOption();
}

bool checkArguments(QCommandLineParser& parser)
{
    //TODO: Refine this function to have a message system and return better status. In theory we want to provide infomation
    //about what is wrong to the caller and let them display it as they want (e.g. stderr, logfiles, message box etc, silent).

    if (!parser.parse(QCoreApplication::arguments()))
    {
        outputArgumentErrorOrHelp(parser.errorText());
    }

    if (parser.isSet("version"))
    {
        outputArgumentErrorOrHelp(QCoreApplication::applicationVersion());
        return false;
    }

    if (parser.isSet("help"))
    {
        outputArgumentErrorOrHelp(parser.helpText());
        return false;
    }

    if (!parser.isSet("input"))
    {
        outputArgumentErrorOrHelp("Error: the --input parameter is required.");
        return false;
    }

    if (parser.isSet("silent") && parser.isSet("verbose"))
    {
        outputArgumentErrorOrHelp("Error: either --silent or --verbose may be used, but not both.");
    }


    const QString inputFile = parser.value("input");
    //Check the input file
    QFileInfo inputInfo(inputFile);
    if (!inputInfo.exists())
    {
        outputArgumentErrorOrHelp("Error: The input resource file "+inputFile+" does not exist.");
        return false;
    }

    if (!inputInfo.isReadable())
    {
        outputArgumentErrorOrHelp("Error: The input resource file "+inputFile+" is not readable.");
        return false;
    }
    if (inputInfo.size()== 0)
    {
        outputArgumentErrorOrHelp("Error: The input resource file "+inputFile+" is empty.");
        return false;
    }

    if (parser.isSet("header"))
    {
        const QString headerFile = parser.value("header");
        QFileInfo headerInfo(headerFile);
        if (!headerInfo.isWritable())
        {
             outputArgumentErrorOrHelp("Error: The header file "+headerFile+" is not writeable.");
            return false;
        }
    }

    if (parser.isSet("translation"))
    {
        const QString translationFile = parser.value("translation");
        QFileInfo translationInfo(translationFile);
        if (!translationInfo.isWritable())
        {
            outputArgumentErrorOrHelp("Error: The header file "+translationFile+" is not writeable.");
            return false;
        }
    }

   return true;
}
