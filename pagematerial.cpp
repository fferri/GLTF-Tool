#include "pagematerial.h"
#include "mainwindow.h"
#include "ui_pagematerial.h"
#include "tinygltf/tiny_gltf.h"
#include "gltfinfo.h"

#include <QColor>
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

void setColorSwatch(QLabel *label, double red, double green, double blue)
{
    QPalette palette = label->palette();
    palette.setColor(QPalette::Window, QColor::fromRgbF(qBound(0.0, red, 1.0),
                                                      qBound(0.0, green, 1.0),
                                                      qBound(0.0, blue, 1.0)));
    label->setAutoFillBackground(true);
    label->setPalette(palette);
}
}

PageMaterial::PageMaterial(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageMaterial)
{
    ui->setupUi(this);

    ui->textures->setColumnCount(4);
    ui->textures->setSelectionMode(QAbstractItemView::NoSelection);
    ui->textures->setHorizontalHeaderLabels(QStringList() << "Slot" << "Texture" << "Tex coord" << "Extra");
    ui->textures->verticalHeader()->setVisible(false);
    ui->textures->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->textures->horizontalHeader()->setMinimumSectionSize(80);
    ui->textures->horizontalHeader()->setStretchLastSection(true);
}

PageMaterial::~PageMaterial()
{
    delete ui;
}

void PageMaterial::setData(const tinygltf::Model &model, int index)
{
    if(index < 0 || index >= (int)model.materials.size()) return;

    const tinygltf::Material &material = model.materials[index];
    const tinygltf::PbrMetallicRoughness &pbr = material.pbrMetallicRoughness;

    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(material.name));
    ui->alphaMode->setText(QString::fromStdString(material.alphaMode));
    ui->alphaCutoff->setText(QString::number(material.alphaCutoff));
    ui->doubleSided->setText(material.doubleSided ? "Yes" : "No");

    QString baseColor = GLTFInfo::vec2str(pbr.baseColorFactor);
    ui->baseColorFactor->setText(baseColor.isEmpty() ? "1 1 1 1" : baseColor);
    if(pbr.baseColorFactor.size() >= 3)
        setColorSwatch(ui->baseColorPreview, pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2]);
    else
        setColorSwatch(ui->baseColorPreview, 1, 1, 1);

    ui->metallicFactor->setText(QString::number(pbr.metallicFactor));
    ui->roughnessFactor->setText(QString::number(pbr.roughnessFactor));

    QString emissive = GLTFInfo::vec2str(material.emissiveFactor);
    ui->emissiveFactor->setText(emissive.isEmpty() ? "0 0 0" : emissive);
    if(material.emissiveFactor.size() >= 3)
        setColorSwatch(ui->emissivePreview, material.emissiveFactor[0], material.emissiveFactor[1], material.emissiveFactor[2]);
    else
        setColorSwatch(ui->emissivePreview, 0, 0, 0);

    QStringList extensions;
    for(const auto &extension : material.extensions)
        extensions << QString::fromStdString(extension.first);
    for(const auto &extension : pbr.extensions)
        extensions << QString::fromStdString(extension.first);
    extensions.removeDuplicates();
    extensions.sort();
    ui->extensions->setText(extensions.isEmpty() ? "None" : extensions.join(", "));

    struct TextureRow
    {
        QString slot;
        int texture;
        int texCoord;
        QString extra;
    };

    QVector<TextureRow> rows;
    auto addTexture = [&rows](const QString &slot, int texture, int texCoord, const QString &extra = QString())
    {
        if(texture >= 0) rows.append({slot, texture, texCoord, extra});
    };

    addTexture("Base color", pbr.baseColorTexture.index, pbr.baseColorTexture.texCoord);
    addTexture("Metallic/roughness", pbr.metallicRoughnessTexture.index, pbr.metallicRoughnessTexture.texCoord);
    addTexture("Normal", material.normalTexture.index, material.normalTexture.texCoord,
               QString("scale %1").arg(material.normalTexture.scale));
    addTexture("Occlusion", material.occlusionTexture.index, material.occlusionTexture.texCoord,
               QString("strength %1").arg(material.occlusionTexture.strength));
    addTexture("Emissive", material.emissiveTexture.index, material.emissiveTexture.texCoord);

    QTableWidget *table = ui->textures;
    table->setRowCount(0);
    table->setRowCount(rows.size());
    for(int row = 0; row < rows.size(); row++)
    {
        const TextureRow &texture = rows[row];
        table->setItem(row, 0, new QTableWidgetItem(texture.slot));
        table->setCellWidget(row, 1, makeLinkLabel(table, mainWindow(), GLTFModel::Textures, texture.texture));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(texture.texCoord)));
        table->setItem(row, 3, new QTableWidgetItem(texture.extra));
    }
    table->clearSelection();
    table->setCurrentCell(-1, -1);
}
