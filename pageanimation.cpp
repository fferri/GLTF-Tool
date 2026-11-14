#include "pageanimation.h"
#include "mainwindow.h"
#include "ui_pageanimation.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

#include <QHeaderView>

namespace
{
QLabel *makeLinkLabel(QWidget *parent, MainWindow *mainWindow, GLTFModel::Group group, int index)
{
    QLabel *label = new QLabel(parent);
    label->setText(mainWindow->link(group, index));
    label->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
    QObject::connect(label, &QLabel::linkActivated, mainWindow, &MainWindow::selectItemByLink);
    return label;
}
}

PageAnimation::PageAnimation(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageAnimation)
{
    ui->setupUi(this);

    ui->tracks->setColumnCount(9);
    ui->tracks->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tracks->setHorizontalHeaderLabels(QStringList()
        << "#" << "Node" << "Path" << "Sampler" << "Interpolation"
        << "Input" << "Output" << "Keys" << "Time range");
    ui->tracks->verticalHeader()->setVisible(false);
    ui->tracks->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tracks->horizontalHeader()->setStretchLastSection(true);
}

PageAnimation::~PageAnimation()
{
    delete ui;
}

void PageAnimation::setData(const tinygltf::Model &model, int index)
{
    if(index < 0 || index >= (int)model.animations.size()) return;

    const tinygltf::Animation &animation = model.animations[index];
    QVector<GLTFInfo::AnimationTrack> tracks = GLTFInfo::animationTracks(&model, index);

    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(animation.name));
    ui->channels->setText(QString::number(animation.channels.size()));
    ui->samplers->setText(QString::number(animation.samplers.size()));

    bool hasTimeRange = false;
    double timeMin = 0;
    double timeMax = 0;
    for(const GLTFInfo::AnimationTrack &track : tracks)
    {
        if(!track.hasTimeRange) continue;
        if(!hasTimeRange)
        {
            timeMin = track.timeMin;
            timeMax = track.timeMax;
            hasTimeRange = true;
        }
        else
        {
            timeMin = qMin(timeMin, track.timeMin);
            timeMax = qMax(timeMax, track.timeMax);
        }
    }
    ui->duration->setText(hasTimeRange
        ? QString("%1 - %2 s").arg(timeMin).arg(timeMax)
        : QString("unknown"));

    // One row per channel, so every track of the animation is visible.
    QTableWidget *table = ui->tracks;
    table->setRowCount(0);
    table->setRowCount(tracks.size());

    for(int row = 0; row < tracks.size(); row++)
    {
        const GLTFInfo::AnimationTrack &track = tracks[row];

        table->setItem(row, 0, new QTableWidgetItem(QString::number(track.channel)));
        table->setItem(row, 2, new QTableWidgetItem(track.path));
        table->setItem(row, 3, new QTableWidgetItem(QString::number(track.sampler)));
        table->setItem(row, 4, new QTableWidgetItem(track.interpolation));
        table->setItem(row, 7, new QTableWidgetItem(QString::number(track.keyCount)));
        table->setItem(row, 8, new QTableWidgetItem(track.hasTimeRange
            ? QString("%1 - %2 s").arg(track.timeMin).arg(track.timeMax)
            : QString()));

        table->setCellWidget(row, 1, makeLinkLabel(table, mainWindow(), GLTFModel::Nodes, track.node));
        table->setCellWidget(row, 5, makeLinkLabel(table, mainWindow(), GLTFModel::Accessors, track.inputAccessor));
        table->setCellWidget(row, 6, makeLinkLabel(table, mainWindow(), GLTFModel::Accessors, track.outputAccessor));
    }
    table->clearSelection();
    table->setCurrentCell(-1, -1);
}
